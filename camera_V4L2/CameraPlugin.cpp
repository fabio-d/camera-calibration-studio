#include "camera_V4L2/CameraPlugin.h"

namespace ccs::camera_V4L2
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
