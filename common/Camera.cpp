#include "common/Camera.h"

#include <QJsonDocument>

namespace ccs::common
{

Camera::Camera(SqliteDatabase *db, int cameraId)
: m_db(db)
, m_cameraId(cameraId)
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

}
