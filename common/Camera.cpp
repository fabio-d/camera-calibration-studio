#include "common/Camera.h"

#include "common/CameraPlugin.h"
#include "common/LiveCapture.h"

#include <QDebug>
#include <QJsonDocument>

namespace ccs::common
{

Camera::Camera(SqliteDatabase *db, int cameraId)
: m_db(db)
, m_cameraId(cameraId)
, m_liveCapture(nullptr)
{
	QSqlRecord queryCamera = m_db->execReturnOne("SELECT * FROM camera WHERE id=?", {cameraId});

	m_name = queryCamera.value("name").toString();
	m_pluginId = queryCamera.value("plugin").toString();
	m_pluginData = QJsonDocument::fromJson(queryCamera.value("plugin_data").toByteArray()).object();

	for (const QSqlRecord &querySensor : m_db->exec("SELECT id FROM sensor WHERE camera_id=?", {cameraId}))
		m_sensors.append(new Sensor(m_db, querySensor.value(0).toInt()));
}

Camera::~Camera()
{
	delete m_liveCapture;
	qDeleteAll(m_sensors);
}

const QString &Camera::name() const
{
	return m_name;
}

const QList<Sensor*> Camera::sensors() const
{
	return m_sensors;
}

CameraStaticInfo Camera::staticInfo() const
{
	CameraStaticInfo r;
	r.pluginId = m_pluginId;
	r.pluginData = m_pluginData;
	r.sensors = {};
	return r;
}

bool Camera::supportsLiveCapture() const
{
	CameraPlugin *plugin = CameraPlugin::pluginInstance(m_pluginId);
	if (plugin != nullptr)
		return plugin->haveLiveCaptureSupport();
	else
		return false;
}

bool Camera::startLiveCapture()
{
	if (m_liveCapture != nullptr)
		return false; // Live capture is already running

	CameraPlugin *plugin = CameraPlugin::pluginInstance(m_pluginId);
	if (plugin == nullptr)
	{
		qCritical() << "Plugin" << m_pluginId << "is not available";
		return false;
	}
	else
	{
		m_liveCapture = plugin->startLiveCapture(m_pluginData);
		if (m_liveCapture == nullptr)
		{
			qCritical() << "Plugin" << m_pluginId << "failed to start:" << m_pluginData;
			return false;
		}
	}

	// The LiveCapture will either be explicitly deleted by the user to stop
	// the live capture session (see stopLiveCapture method), or it will
	// self-delete in case of camera acquisition errors.
	connect(m_liveCapture, &QObject::destroyed, this, &Camera::liveCaptureDestroyed);

	emit liveCaptureChanged();
	return true;
}

void Camera::stopLiveCapture()
{
	// m_liveCapture will be reset to nullptr by slot liveCaptureDestroyed
	delete m_liveCapture;
}

void Camera::liveCaptureDestroyed()
{
	m_liveCapture = nullptr;
	emit liveCaptureChanged();
}

LiveCapture *Camera::liveCapture() const
{
	return m_liveCapture;
}

}
