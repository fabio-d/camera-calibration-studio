#pragma once

#include "common/Camera.h"
#include "common/Pattern.h"
#include "common/Sensor.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QWidget>

namespace ccs::main
{

class Ui_CentralWidget;

class CentralWidget : public QWidget
{
	Q_OBJECT

	public:
		CentralWidget(QWidget *parent);
		~CentralWidget();

		void showNothing();
		void showLiveCapture(common::Camera *camera, common::Sensor *sensor, common::Sensor::ImageType imageType);
		void showShot(common::Shot *shot, common::Sensor *sensor, common::Sensor::ImageType imageType);
		void showPattern(common::Pattern *pattern);

		void updateImage();

	private:
		void resizeEvent(QResizeEvent *event) override;

		void updateZoom();
		void drawChessboardCorners(const cv::Size &patternSize, const std::vector<cv::Point2f> &corners);

		Ui_CentralWidget *m_ui;

		QGraphicsScene *m_scene;
		QGraphicsPixmapItem *m_pixmapItem;
		QList<QGraphicsPathItem*> m_chessboardPaths;

		common::Sensor::ImageType m_imageType; // Invalid = showNothing

		// showLiveCapture AND showShot
		common::Sensor *m_selectedSensor;
		QMetaObject::Connection m_calibrationParametersConnection;

		// showLiveCapture
		common::Camera *m_liveCaptureCamera;
		QMetaObject::Connection m_liveCaptureNewFrameConnection;

		// showShot
		common::Shot *m_shot;

		//showPattern
		common::Pattern *m_pattern;
		QMetaObject::Connection m_patternGeometryChanged;
};

}
