#include "common/CameraPlugin.h"

#include <QApplication>
#include <QPluginLoader>
#include <QDir>

namespace ccs::common
{

static QList<CameraPluginMetadata> pluginList;
static QMap<QString, QPluginLoader*> pluginLoaderMap;

void CameraPlugin::detectAvailablePlugins(const QString &searchPath)
{
	QDir pluginsDir(searchPath);

	for (const QString &fileName : pluginsDir.entryList(QDir::Files))
	{
		if (!fileName.startsWith("ccscamera_"))
			continue;

		// Load plugin
		QPluginLoader *loader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));

		// Parse JSON metadata
		QJsonObject metadata = loader->metaData()["MetaData"].toObject();
		CameraPluginMetadata m;
		m.pluginId = metadata["id"].toString();
		m.pluginName = metadata["name"].toString();

		pluginList.append(m);
		pluginLoaderMap.insert(m.pluginId, loader);
	}
}

const QList<CameraPluginMetadata> &CameraPlugin::availablePluginsMetadata()
{
	return pluginList;
}

CameraPlugin *CameraPlugin::pluginInstance(const QString &pluginId)
{
	QObject *plugin = pluginLoaderMap.value(pluginId)->instance();
	return qobject_cast<CameraPlugin*>(plugin);
}

CameraPlugin::~CameraPlugin()
{
}

}
