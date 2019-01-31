#include "main/CentralWidget.h"

#include "ui_CentralWidget.h"

#include <QDebug>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
	disconnect(m_patternGeometryChanged);

	m_selectedSensor = nullptr;
	m_pattern = nullptr;

	updateImage();
}

void CentralWidget::showLiveCapture(common::Camera *camera, common::Sensor *sensor, common::Sensor::ImageType imageType)
{
	disconnect(m_liveCaptureNewFrameConnection);
	disconnect(m_calibrationParametersConnection);
	disconnect(m_patternGeometryChanged);

	m_selectedSensor = sensor;
	m_liveCaptureCamera = camera;
	m_shot = nullptr;
	m_imageType = imageType;
	m_pattern = nullptr;

	m_liveCaptureNewFrameConnection = connect(
		camera, &common::Camera::capturedFrameChanged,
		this, &CentralWidget::updateImage);
	m_calibrationParametersConnection = connect(
		sensor, &common::Sensor::calibrationParametersChanged,
		this, &CentralWidget::updateImage);
	updateImage();
}

void CentralWidget::showShot(common::Shot *shot, common::Sensor *sensor, common::Sensor::ImageType imageType)
{
	disconnect(m_liveCaptureNewFrameConnection);
	disconnect(m_calibrationParametersConnection);
	disconnect(m_patternGeometryChanged);

	m_selectedSensor = sensor;
	m_liveCaptureCamera = nullptr;
	m_shot = shot;
	m_imageType = imageType;
	m_pattern = nullptr;

	m_calibrationParametersConnection = connect(
		sensor, &common::Sensor::calibrationParametersChanged,
		this, &CentralWidget::updateImage);
	updateImage();
}

void CentralWidget::showPattern(common::Pattern *pattern)
{
	disconnect(m_liveCaptureNewFrameConnection);
	disconnect(m_calibrationParametersConnection);
	disconnect(m_patternGeometryChanged);

	m_selectedSensor = nullptr;
	m_pattern = pattern;

	m_patternGeometryChanged = connect(
		pattern, &common::Pattern::geometryChanged,
		this, &CentralWidget::updateImage);
	updateImage();
}

void CentralWidget::updateImage()
{
	if (m_selectedSensor != nullptr)
	{
		assert(m_pattern == nullptr);

		cv::Mat sensorImage;
		if (m_imageType != common::Sensor::Invalid)
		{
			if (m_liveCaptureCamera != nullptr)
			{
				assert(m_shot == nullptr);
				sensorImage = m_liveCaptureCamera->lastCapturedFrame().value(m_selectedSensor, cv::Mat());
			}
			else
			{
				assert(m_shot != nullptr);
				sensorImage = m_shot->sensorData(m_selectedSensor);

				auto tmp = m_shot->patternData(m_selectedSensor);
				if (tmp.first != nullptr)
				{
					cv::Size patternSize(tmp.first->cornerCountX(), tmp.first->cornerCountY());
					if (!tmp.second.empty())
						cv::drawChessboardCorners(sensorImage, patternSize, tmp.second, true);
				}
			}
		}

		if (sensorImage.empty())
			m_ui->label->setPixmap(QPixmap());
		else
			m_ui->label->setPixmap(QPixmap::fromImage(m_selectedSensor->renderImage(sensorImage, m_imageType)));
	}
	else if (m_pattern != nullptr)
	{
		assert(m_selectedSensor == nullptr);

		double fullSizeX = m_pattern->cornerDistanceX() * (m_pattern->cornerCountX() + 1);
		double fullSizeY = m_pattern->cornerDistanceY() * (m_pattern->cornerCountY() + 1);
		double multiplier = 600 / qMax(fullSizeX, fullSizeY);

		bool invert = false;
		cv::Mat pixelated(m_pattern->cornerCountY() + 1, m_pattern->cornerCountX() + 1, CV_8UC1);
		for (int r = 0; r < pixelated.rows; r++)
			for (int c = 0; c < pixelated.cols; c++)
				pixelated.at<uint8_t>(r, c) = invert == !((r+c)%2) ? 255 : 0;

		cv::Mat result;
		cv::resize(pixelated, result, cv::Size(fullSizeX * multiplier, fullSizeY * multiplier), 0, 0, cv::INTER_NEAREST);

		int borderPx = 20;
		copyMakeBorder(result, result, borderPx, borderPx, borderPx, borderPx, cv::BORDER_CONSTANT, 255);

		QImage img(result.cols, result.rows, QImage::Format_RGB888);
		cv::Mat qtImg(img.height(), img.width(), CV_8UC3, img.bits(), img.bytesPerLine());
		cv::cvtColor(result, qtImg, cv::COLOR_GRAY2RGB);

		m_ui->label->setPixmap(QPixmap::fromImage(img));
	}
	else
	{
		m_ui->label->setPixmap(QPixmap());
	}
}

}
