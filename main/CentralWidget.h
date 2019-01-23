#pragma once

#include "common/Camera.h"
#include "common/Sensor.h"

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

	private:
		void updateImage();

		Ui_CentralWidget *m_ui;

		common::Sensor::ImageType m_imageType; // Invalid = showNothing

		common::Camera *m_liveCaptureCamera;
		common::Sensor *m_liveCaptureSensor;
		QMetaObject::Connection m_liveCaptureNewFrameConnection;
		QMetaObject::Connection m_calibrationParametersConnection;
};

}
