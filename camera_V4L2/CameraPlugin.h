#pragma once

#include "camera_V4L2/AddCameraWidget.h"

#include "common/CameraPlugin.h"

namespace ccs::camera_V4L2
{

class CameraPlugin : public QObject, public common::CameraPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID CameraPlugin_iid FILE "camera_V4L2.json")
	Q_INTERFACES(ccs::common::CameraPlugin)

	public:
		AddCameraWidget *createAddCameraWidget(QWidget *parent) const override;
};

}
