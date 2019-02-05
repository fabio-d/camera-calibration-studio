#include "common/CameraPlugin.h"

#include <QApplication>
#include <QPluginLoader>
#include <QDir>

namespace ccs::common
{

static QList<CameraPluginMetadata> pluginList;
static QMap<QString, QPluginLoader*> pluginLoaderMap;

static bool pluginLessThan(const CameraPluginMetadata &a, const CameraPluginMetadata &b)
{
	return a.listPriority < b.listPriority;
}

void CameraPlugin::detectAvailablePlugins(const QString &searchPath)
{
	QDir pluginsDir(searchPath);

	for (const QString &fileName : pluginsDir.entryList(QDir::Files, QDir::Name))
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
		m.listPriority = metadata["listPriority"].toInt();

		pluginList.append(m);
		pluginLoaderMap.insert(m.pluginId, loader);
	}

	qStableSort(pluginList.begin(), pluginList.end(), pluginLessThan);
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
