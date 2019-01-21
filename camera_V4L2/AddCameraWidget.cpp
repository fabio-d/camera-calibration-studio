#include "camera_V4L2/AddCameraWidget.h"

#include "common/Sensor.h"

#include <QDir>

#include <fcntl.h>
#include <libv4l2.h>
#include <linux/videodev2.h>

#include "ui_AddCameraWidget.h"

#define V4L_BYID_DIR "/dev/v4l/by-id"

namespace ccs::camera_V4L2
{

AddCameraWidget::AddCameraWidget(QWidget *parent)
: common::AddCameraWidget(parent)
, m_ui(new Ui_AddCameraWidget)
, m_validDevice(false)
{
	m_ui->setupUi(this);

	connect(m_ui->rescanPushButton, &QPushButton::clicked, this, &AddCameraWidget::rescanDevices);
	connect(m_ui->pathComboBox, &QComboBox::currentTextChanged, this, &AddCameraWidget::deviceSelected);

	rescanDevices();
}

AddCameraWidget::~AddCameraWidget()
{
	delete m_ui;
}

QJsonObject AddCameraWidget::pluginData()
{
	QSize resolution = m_ui->resolutionComboBox->currentData().toSize();

	QJsonObject r;
	r["devicePath"] = m_devicePath;
	r["cardName"] = m_cardString.constData();
	r["driver"] = m_driverString.constData();

	r["width"] = resolution.width();
	r["height"] = resolution.height();

	return r;
}

QList<common::SensorStaticInfo> AddCameraWidget::sensorsStaticInfo()
{
	QSize resolution = m_ui->resolutionComboBox->currentData().toSize();

	common::SensorStaticInfo s;
	s.name = "RGB";
	s.width = resolution.width();
	s.height = resolution.height();

	return QList<common::SensorStaticInfo>() << s;
}

QString AddCameraWidget::defaultCameraName() const
{
	return m_ui->cardLabel->text();
}

bool AddCameraWidget::canAccept() const
{
	return m_validDevice;
}

void AddCameraWidget::rescanDevices()
{
	QDir v4lDir(V4L_BYID_DIR);

	const QString oldText = m_ui->pathComboBox->currentText();
	m_ui->pathComboBox->clear();
	m_ui->pathComboBox->addItems(v4lDir.entryList(QDir::Files));

	if (oldText.isEmpty() == false)
		m_ui->pathComboBox->setCurrentText(oldText);

	deviceSelected();

	if (m_ui->pathComboBox->currentText().isEmpty())
		m_ui->validOrNotStackedWidget->setCurrentWidget(m_ui->noDevicePage);
}

void AddCameraWidget::deviceSelected()
{
	QDir v4lDir(V4L_BYID_DIR);

	m_devicePath = v4lDir.absoluteFilePath(m_ui->pathComboBox->currentText());
	m_validDevice = false;

	if (m_ui->pathComboBox->currentText().isEmpty() == false && QFileInfo(m_devicePath).isWritable())
	{
		QByteArray nativeDevicePath = m_devicePath.toLocal8Bit();

		int fd = v4l2_open(nativeDevicePath.constData(), O_RDWR);
		if (fd != -1)
		{
			m_validDevice = loadDeviceInfo(fd);
			v4l2_close(fd);
		}
	}

	m_ui->resolutionComboBox->clear();

	if (m_validDevice)
	{
		m_ui->validOrNotStackedWidget->setCurrentWidget(m_ui->validDevicePage);
		m_ui->cardLabel->setText(m_cardString.constData());
		m_ui->busInfoLabel->setText(m_busInfoString.constData());
		m_ui->driverLabel->setText(m_driverString.constData());

		for (const QSize &s : m_supportedResolutions)
		{
			QString text = QString("%1x%2").arg(s.width()).arg(s.height());
			if (m_ui->resolutionComboBox->findText(text) == -1)
				m_ui->resolutionComboBox->addItem(text, s);
		}
	}
	else
	{
		m_ui->validOrNotStackedWidget->setCurrentWidget(m_ui->invalidDevicePage);
		m_ui->cardLabel->clear();
		m_ui->busInfoLabel->clear();
		m_ui->driverLabel->clear();
	}

	emit defaultCameraNameChanged();
	emit canAcceptChanged();
}

bool AddCameraWidget::loadDeviceInfo(int fd)
{
	v4l2_capability cap;
	memset(&cap, 0, sizeof(cap));
	if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
		return false;

	// Check capabilities
	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0)
		return false;
	if ((cap.capabilities & V4L2_CAP_DEVICE_CAPS) != 0 &&
		(cap.device_caps & V4L2_CAP_VIDEO_CAPTURE) == 0)
		return false;

	m_cardString = QByteArray((const char*)cap.card, sizeof(cap.card));
	m_busInfoString = QByteArray((const char*)cap.bus_info, sizeof(cap.bus_info));
	m_driverString = QByteArray((const char*)cap.driver, sizeof(cap.driver));

	v4l2_frmsizeenum frmsize;
	memset(&frmsize, 0, sizeof(frmsize));
	frmsize.pixel_format = V4L2_PIX_FMT_BGR24;
	frmsize.index = 0;

	while (true)
	{
		if (v4l2_ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) < 0)
			break;

		if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
		{
			m_supportedResolutions.append(QSize(frmsize.discrete.width, frmsize.discrete.height));
			frmsize.index++;
		}
		else
		{
			m_supportedResolutions.append(QSize(frmsize.stepwise.max_width, frmsize.stepwise.max_height));
			break;
		}
	}

	return !m_supportedResolutions.isEmpty();
}

}
