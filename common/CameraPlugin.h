#pragma once

#include <QObject>

#define CameraPlugin_iid "com.example.ccs.CameraPlugin"

namespace ccs::common
{

class AddCameraWidget;
class LiveCapture;

struct CameraPluginMetadata
{
	QString pluginId;
	QString pluginName;
	int listPriority;
};

class CameraPlugin
{
	public:
		static void detectAvailablePlugins(const QString &searchPath);
		static const QList<CameraPluginMetadata> &availablePluginsMetadata();
		static CameraPlugin *pluginInstance(const QString &pluginId);

		virtual ~CameraPlugin();

		virtual AddCameraWidget *createAddCameraWidget(QWidget *parent) const = 0;

		virtual bool haveLiveCaptureSupport() const = 0;
		virtual LiveCapture *startLiveCapture(const QJsonObject &pluginData) const = 0;
};

}

Q_DECLARE_INTERFACE(ccs::common::CameraPlugin, CameraPlugin_iid)
