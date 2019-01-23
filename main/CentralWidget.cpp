#include "main/CentralWidget.h"

#include "ui_CentralWidget.h"

#include <QDebug>

namespace ccs::main
{

CentralWidget::CentralWidget(QWidget *parent)
: QWidget(parent)
, m_ui(new Ui_CentralWidget)
{
	m_ui->setupUi(this);

	showNothing();
}

CentralWidget::~CentralWidget()
{
	delete m_ui;
}

void CentralWidget::showNothing()
{
	disconnect(m_liveCaptureNewFrameConnection);
	disconnect(m_calibrationParametersConnection);

	m_imageType = common::Sensor::Invalid;
	updateImage();
}

void CentralWidget::showLiveCapture(common::Camera *camera, common::Sensor *sensor, common::Sensor::ImageType imageType)
{
	disconnect(m_liveCaptureNewFrameConnection);
	disconnect(m_calibrationParametersConnection);

	m_liveCaptureCamera = camera;
	m_liveCaptureSensor = sensor;
	m_imageType = imageType;

	m_liveCaptureNewFrameConnection = connect(
		camera, &common::Camera::capturedFrameChanged,
		this, &CentralWidget::updateImage);
	m_calibrationParametersConnection = connect(
		sensor, &common::Sensor::calibrationParametersChanged,
		this, &CentralWidget::updateImage);
	updateImage();
}

void CentralWidget::updateImage()
{
	qCritical() << "HERE";

	cv::Mat sensorImage;
	if (m_imageType != common::Sensor::Invalid)
		sensorImage = m_liveCaptureCamera->lastCapturedFrame().value(m_liveCaptureSensor, cv::Mat());

	if (sensorImage.empty())
		m_ui->label->setPixmap(QPixmap());
	else
		m_ui->label->setPixmap(QPixmap::fromImage(m_liveCaptureSensor->renderImage(sensorImage, m_imageType)));
}

}
