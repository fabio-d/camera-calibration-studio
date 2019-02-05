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

	m_scene = new QGraphicsScene(this);
	m_ui->graphicsView->setScene(m_scene);

	m_pixmapItem = new QGraphicsPixmapItem();
	m_scene->addItem(m_pixmapItem);

	m_ui->zoomComboBox->addItem("Fit",  QVariant(-1.0));
	m_ui->zoomComboBox->addItem("10%",  QVariant(0.1));
	m_ui->zoomComboBox->addItem("20%",  QVariant(0.2));
	m_ui->zoomComboBox->addItem("50%",  QVariant(0.5));
	m_ui->zoomComboBox->addItem("100%", QVariant(1.0));
	m_ui->zoomComboBox->addItem("150%", QVariant(1.5));
	m_ui->zoomComboBox->addItem("200%", QVariant(2.0));
	m_ui->zoomComboBox->addItem("300%", QVariant(3.0));
	m_ui->zoomComboBox->addItem("400%", QVariant(4.0));

	connect(m_ui->zoomComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CentralWidget::updateZoom);

	showNothing();
}

CentralWidget::~CentralWidget()
{
	delete m_pixmapItem;
	qDeleteAll(m_chessboardPaths);

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
	qDeleteAll(m_chessboardPaths);
	m_chessboardPaths.clear();

	QPixmap pixmap;

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

				if (m_imageType == common::Sensor::Original)
				{
					auto tmp = m_shot->patternData(m_selectedSensor);
					if (tmp.first != nullptr)
					{
						cv::Size patternSize(tmp.first->cornerCountX(), tmp.first->cornerCountY());
						if (!tmp.second.empty())
							drawChessboardCorners(patternSize, tmp.second);
					}
				}
			}
		}

		if (!sensorImage.empty())
			pixmap = QPixmap::fromImage(m_selectedSensor->renderImage(sensorImage, m_imageType));
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

		pixmap = QPixmap::fromImage(img);
	}

	m_pixmapItem->setPixmap(pixmap);
	updateZoom();
}

void CentralWidget::resizeEvent(QResizeEvent *event)
{
	updateZoom();
}

void CentralWidget::updateZoom()
{
	double scale = m_ui->zoomComboBox->currentData().toDouble();
	if (m_pixmapItem->pixmap().isNull())
	{
		m_ui->graphicsView->setSceneRect(0, 0, 1, 1);
		m_ui->graphicsView->setTransform(QTransform());
	}
	else
	{
		m_ui->graphicsView->setSceneRect(m_pixmapItem->boundingRect());
		if (scale < 0)
			m_ui->graphicsView->fitInView(m_ui->graphicsView->sceneRect(), Qt::KeepAspectRatio);
		else
			m_ui->graphicsView->setTransform(QTransform::fromScale(scale, scale));
	}
}

// same shape and colors as cv::drawChessboardCorners
void CentralWidget::drawChessboardCorners(const cv::Size &patternSize, const std::vector<cv::Point2f> &corners)
{
	static QVector<QRgb> palette
	{
		qRgb(255, 0, 0), qRgb(255, 128, 0), qRgb(200, 200, 0),
		qRgb(0, 255, 0), qRgb(0, 200, 200), qRgb(0, 0, 255),
		qRgb(255, 0, 255)
	};

	QPainterPath marker;
	marker.moveTo(-4, -4);
	marker.lineTo(+4, +4);
	marker.moveTo(-4, +4);
	marker.lineTo(+4, -4);
	marker.addEllipse(QPointF(0, 0), 5, 5);

	int i = 0;

	for (int y = 0; y < patternSize.height; y++)
	{
		QPainterPath p;

		for (int x = 0; x < patternSize.width; x++)
		{
			if (i != 0)
			{
				p.moveTo(corners[i-1].x, corners[i-1].y);
				p.lineTo(corners[i].x, corners[i].y);
			}

			p.addPath(marker.translated(corners[i].x, corners[i].y));

			i++;
		}

		QGraphicsPathItem *it = new QGraphicsPathItem(p);
		it->setPen(QPen(QColor::fromRgb(palette[y % palette.count()]), 0));

		m_scene->addItem(it);
		m_chessboardPaths << it;
	}
}

}
