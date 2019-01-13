#include "common/Project.h"

#include "common/Camera.h"

#include <QJsonDocument>

namespace ccs::common
{

Project *Project::createOrOpen(const QString &filePath)
{
	QScopedPointer<SqliteDatabase> db(new SqliteDatabase(filePath));

	if (!db->isOpenOk()) // failed to open file
		return nullptr;

	return new Project(db.take());
}

Project::Project(SqliteDatabase *db)
: m_db(db)
{
	for (const QSqlRecord &cameraRecord : m_db->exec("SELECT id FROM camera"))
		m_cameras.insert(new Camera(m_db, cameraRecord.value(0).toInt()));
}

Project::~Project()
{
	qDeleteAll(m_cameras);

	delete m_db;
}

const QSet<Camera*> &Project::cameras() const
{
	return m_cameras;
}

Camera *Project::addCamera(const QString &cameraName, const CameraStaticInfo &info)
{
	int camId = m_db->execInsertId("INSERT INTO camera(name, plugin, plugin_data) VALUES (?, ?, ?)",
		{cameraName, info.pluginId, QJsonDocument(info.pluginData).toJson()});

	for (const SensorStaticInfo &s : info.sensors)
	{
		m_db->exec("INSERT INTO sensor(camera_id, name, width, height, calibration_parameters) VALUES (?, ?, ?, ?, ?)",
			{camId, s.name, s.width, s.height, QJsonDocument(QJsonObject()).toJson()});
	}

	Camera *camera = new Camera(m_db, camId);
	m_cameras.insert(camera);
	emit cameraAdded(camera);

	return camera;
}

void Project::removeCamera(Camera *camera)
{
	m_db->exec("DELETE FROM sensor WHERE camera_id=?", {camera->m_cameraId});
	m_db->exec("DELETE FROM camera WHERE id=?", {camera->m_cameraId});

	m_cameras.remove(camera);
	emit cameraRemoved(camera);

	delete camera;
}

}
