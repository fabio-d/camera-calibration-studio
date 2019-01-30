#include "camera_gazebo/GazeboConnection.h"

#include <opencv2/imgproc/imgproc.hpp>

#include <QDebug>

#define CONNECTION_TIMEOUT	2 /* seconds */

namespace ccs::camera_gazebo
{

GazeboConnection::~GazeboConnection()
{
	qCritical() << "FINI";
	
	gazebo::transport::fini();
}

QSharedPointer<GazeboConnection> GazeboConnection::connectToGazebo()
{
	static QWeakPointer<GazeboConnection> conn;

	QSharedPointer<GazeboConnection> result(conn);
	if (result.isNull())
	{
		qCritical() << "INIT";

		// Connect to running gzserver
		if (gazebo::transport::init("", 0, CONNECTION_TIMEOUT))
		{
			gazebo::transport::run();

			result.reset(new GazeboConnection());
			conn = result;
		}
	}

	return result;
}

QStringList GazeboConnection::imageStampedTopicList() const
{
	QStringList r;

	for (const std::string &n : gazebo::transport::getAdvertisedTopics("gazebo.msgs.ImageStamped"))
		r.append(n.c_str());

	return r;
}

GazeboSubscription::GazeboSubscription(const QSharedPointer<GazeboConnection> &conn, const QString &topicName)
: m_conn(conn)
, m_node(new gazebo::transport::Node())
{
	qCritical() << "SUB" << topicName;

	m_node->Init();
	m_sub = m_node->Subscribe(topicName.toLatin1().constData(),
		&GazeboSubscription::onMessage, this);
}

GazeboSubscription::~GazeboSubscription()
{
	qCritical() << "UNSUB";
}

void GazeboSubscription::onMessage(ConstImageStampedPtr &msg)
{
	gazebo::common::Image img;
	gazebo::msgs::Set(img, msg->image());

	unsigned char *data = nullptr;
	unsigned int count;

	img.GetRGBData(&data, count);
	cv::Mat imgRGB, imgBGR(img.GetHeight(), img.GetWidth(), CV_8UC3, data, img.GetWidth() * 3);
	cv::cvtColor(imgBGR, imgRGB, cv::COLOR_RGB2BGR);

	delete[] data;

	//qCritical() << imgRGB.rows << imgRGB.cols;

	m_enqueuedFrameMutex.lock();
	m_enqueuedFrame = imgRGB;
	m_enqueuedFrameMutex.unlock();

	QMetaObject::invokeMethod(this, "frameEnqueued", Qt::QueuedConnection);
}

void GazeboSubscription::frameEnqueued()
{
	cv::Mat frame;
	m_enqueuedFrameMutex.lock();
	cv::swap(m_enqueuedFrame, frame);
	m_enqueuedFrameMutex.unlock();

	emit frameReceived(frame);
}

}
