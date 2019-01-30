#pragma once

#include "camera_gazebo/AddCameraWidget.h"
#include "camera_gazebo/LiveCapture.h"

#include "common/CameraPlugin.h"

namespace ccs::camera_gazebo
{

class CameraPlugin : public QObject, public common::CameraPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID CameraPlugin_iid FILE "camera_gazebo.json")
	Q_INTERFACES(ccs::common::CameraPlugin)

	public:
		AddCameraWidget *createAddCameraWidget(QWidget *parent) const override;

		bool haveLiveCaptureSupport() const override;
		LiveCapture *startLiveCapture(const QJsonObject &pluginData) const override;
};

}
