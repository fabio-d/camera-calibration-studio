#include "camera_V4L2/LiveCapture.h"

#include "camera_V4L2/LiveCaptureParameters.h"

#include <QDebug>
#include <QJsonObject>

#include <fcntl.h>
#include <libv4l2.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

#define NUM_BUFFERS	2
#define NUM_CHANNELS	3 // BGR

namespace ccs::camera_V4L2
{

LiveCapture::LiveCapture(const QJsonObject &pluginData)
: m_fdMonitor(nullptr)
{
	QByteArray nativeDevicePath = pluginData.value("devicePath").toString().toLocal8Bit();
	m_imageSize.width = pluginData.value("width").toInt();
	m_imageSize.height = pluginData.value("height").toInt();

	m_fd = v4l2_open(nativeDevicePath.constData(), O_RDWR | O_NONBLOCK);
	if (m_fd == -1 || !checkDeviceMatches(pluginData) || !configureDevice())
	{
		qCritical() << "Failed to open, verify or configure" << nativeDevicePath;
		deleteLater();
		return;
	}

	mapBuffers();

	qCritical() << "STARTED";
	m_fdMonitor = new QSocketNotifier(m_fd, QSocketNotifier::Read);
	connect(m_fdMonitor, &QSocketNotifier::activated, this, &LiveCapture::frameCallback);

	streamOn();

	// Enumerate user-settable controls
	v4l2_queryctrl queryctrl;
	memset(&queryctrl, 0, sizeof(queryctrl));
	queryctrl.id = V4L2_CTRL_FLAG_NEXT_CTRL;
	while (v4l2_ioctl(m_fd, VIDIOC_QUERYCTRL, &queryctrl) == 0)
	{
		common::BaseLiveCaptureParameter *p = createParameterObject(m_fd, queryctrl.id);
		if (p != nullptr)
		{
			m_parameters.insert(queryctrl.id, p);

			v4l2_event_subscription event_subscription;
			memset(&event_subscription, 0, sizeof(event_subscription));
			event_subscription.type = V4L2_EVENT_CTRL;
			event_subscription.id = queryctrl.id;

			if (v4l2_ioctl(m_fd, VIDIOC_SUBSCRIBE_EVENT, &event_subscription) < 0)
				qCritical() << "Failed to subscribe event" << queryctrl.id;
		}

		queryctrl.id |= V4L2_CTRL_FLAG_NEXT_CTRL;
	}
	if (errno != EINVAL)
		qCritical() << "Failed to enumerate controls" << nativeDevicePath;
}

LiveCapture::~LiveCapture()
{
	qCritical() << "STOPPING";

	qDeleteAll(m_parameters);

	unmapBuffers();

	delete m_fdMonitor;

	if (m_fd != -1)
		v4l2_close(m_fd);
}

QList<common::BaseLiveCaptureParameter*> LiveCapture::parameterList() const
{
	return m_parameters.values();
}

bool LiveCapture::checkDeviceMatches(const QJsonObject &pluginData)
{
	v4l2_capability cap;
	memset(&cap, 0, sizeof(cap));
	if (v4l2_ioctl(m_fd, VIDIOC_QUERYCAP, &cap) < 0)
		return false;

	// Check capabilities
	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
		return false;
	if ((cap.capabilities & V4L2_CAP_DEVICE_CAPS) != 0 &&
		(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE) == 0)
		return false;

	QByteArray cardString((const char*)cap.card, sizeof(cap.card));
	QByteArray busInfoString((const char*)cap.bus_info, sizeof(cap.bus_info));
	QByteArray driverString((const char*)cap.driver, sizeof(cap.driver));

	if (cardString.constData() != pluginData.value("cardName").toString())
		return false;
	if (driverString.constData() != pluginData.value("driver").toString())
		return false;

	return true;
}

bool LiveCapture::configureDevice()
{
	// Set video format
	v4l2_format format;
	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = m_imageSize.width;
	format.fmt.pix.height = m_imageSize.height;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
	format.fmt.pix.field = V4L2_FIELD_NONE;
	if (v4l2_ioctl(m_fd, VIDIOC_S_FMT, &format) < 0)
		return false;

	// Check actual video format
	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (v4l2_ioctl(m_fd, VIDIOC_G_FMT, &format) < 0
		|| format.fmt.pix.width != m_imageSize.width
		|| format.fmt.pix.height != m_imageSize.height
		|| format.fmt.pix.pixelformat != V4L2_PIX_FMT_BGR24)
	{
		return false;
	}

	m_bytesPerLine = format.fmt.pix.bytesperline;

	// Create buffers on device memory
	v4l2_requestbuffers requestbuffers;
	memset(&requestbuffers, 0, sizeof(requestbuffers));
	requestbuffers.count = NUM_BUFFERS;
	requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	requestbuffers.memory = V4L2_MEMORY_MMAP;
	if (v4l2_ioctl(m_fd, VIDIOC_REQBUFS, &requestbuffers) < 0 || requestbuffers.count != NUM_BUFFERS)
		return false;

	return true;
}

void LiveCapture::mapBuffers()
{
	// mmap() buffers to user memory
	for (unsigned int i = 0; i < NUM_BUFFERS; i++)
	{
		v4l2_buffer buffer;
		memset(&buffer, 0, sizeof(buffer));
		buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = i;
		if (v4l2_ioctl(m_fd, VIDIOC_QUERYBUF, &buffer) < 0)
			qFatal("VIDIOC_QUERYBUF failed");

		unsigned int expectedLength = m_imageSize.height * m_bytesPerLine;
		if ((unsigned int)buffer.length < expectedLength)
		{
			qFatal("Buffer is too small (got %u, expected %u)",
			     (unsigned int)buffer.length, expectedLength);
		}

		void *b = v4l2_mmap(nullptr, buffer.length, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, buffer.m.offset);
		if (b == MAP_FAILED)
			qFatal("v4l2_mmap failed");
	
		if (v4l2_ioctl(m_fd, VIDIOC_QBUF, &buffer) < 0)
			qFatal("VIDIOC_QBUF failed, index %d", buffer.index);

		m_buffers.append(b);
		m_bufferSize.insert(b, buffer.length);
	}
}

void LiveCapture::unmapBuffers()
{
	for (QMap<void*, size_t>::iterator it = m_bufferSize.begin();
		it != m_bufferSize.end(); ++it)
	{
		v4l2_munmap(it.key(), it.value());
	}
}

void LiveCapture::streamOn()
{
	// Start streaming
	v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (v4l2_ioctl(m_fd, VIDIOC_STREAMON, &buf_type) < 0)
		qFatal("VIDIOC_STREAMON");
}

bool LiveCapture::dequeueNextEvent()
{
	v4l2_event event;
	memset(&event, 0, sizeof(event));
	if (v4l2_ioctl(m_fd, VIDIOC_DQEVENT, &event) < 0)
		return false;

	if (event.type == V4L2_EVENT_CTRL)
	{
		common::BaseLiveCaptureParameter *p = m_parameters.value(event.id, nullptr);
		if (p != nullptr)
			emit p->somethingChanged();
	}

	return true;
}

bool LiveCapture::dequeueNextFrame()
{
	v4l2_buffer buffer;
	memset(&buffer, 0, sizeof(buffer));
	buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buffer.memory = V4L2_MEMORY_MMAP;
	if (v4l2_ioctl(m_fd, VIDIOC_DQBUF, &buffer) < 0)
		return false;

	cv::Mat result = cv::Mat(m_imageSize, CV_8UC3, m_buffers[buffer.index], m_bytesPerLine).clone();

        if (v4l2_ioctl(m_fd, VIDIOC_QBUF, &buffer) < 0)
		return false;

	//qCritical() << "FRAME";
	emit frameCaptured(QList<cv::Mat>() << result);

	return true;
}

void LiveCapture::frameCallback()
{
	bool gotEvent = dequeueNextEvent();
	bool gotFrame = dequeueNextFrame();

	if (!gotEvent && !gotFrame)
	{
		qCritical() << "Read error";
		deleteLater();
	}
}

}
