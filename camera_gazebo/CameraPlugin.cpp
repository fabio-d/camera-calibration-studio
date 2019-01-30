#include "camera_gazebo/CameraPlugin.h"

namespace ccs::camera_gazebo
{

AddCameraWidget *CameraPlugin::createAddCameraWidget(QWidget *parent) const
{
	return new AddCameraWidget(parent);
}

bool CameraPlugin::haveLiveCaptureSupport() const
{
	return true;
}

LiveCapture *CameraPlugin::startLiveCapture(const QJsonObject &pluginData) const
{
	return new LiveCapture(pluginData);
}

}
