#include "common/Project.h"

#include "common/Camera.h"
#include "common/Pattern.h"

#include <QDebug>
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
	connect(m_db, &SqliteDatabase::dirtyStateChanged, this, &Project::dbDirtyStateChanged);

	for (const QSqlRecord &cameraRecord : m_db->exec("SELECT id FROM camera"))
		m_cameras.insert(new Camera(m_db, cameraRecord.value(0).toInt()));

	for (const QSqlRecord &patternRecord : m_db->exec("SELECT id FROM pattern"))
		m_patterns.insert(new Pattern(m_db, patternRecord.value(0).toInt()));
}

Project::~Project()
{
	qDeleteAll(m_cameras);
	qDeleteAll(m_patterns);

	delete m_db;
}

bool Project::isDirtyState() const
{
	return m_db->isDirtyState();
}

void Project::dbDirtyStateChanged()
{
	qCritical() << m_db->isDirtyState();
	emit dirtyStateChanged(m_db->isDirtyState());
}

bool Project::commit()
{
	m_db->commit();
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
	m_db->exec("DELETE FROM camera WHERE id=?", {camera->m_cameraId});

	m_cameras.remove(camera);
	emit cameraRemoved(camera);

	delete camera;
}

const QSet<Pattern*> &Project::patterns() const
{
	return m_patterns;
}

Pattern *Project::addPattern(const QString &patternName, int cornerCountX, int cornerCountY)
{
	int patId = m_db->execInsertId("INSERT INTO pattern(name, corner_count_x, corner_count_y) VALUES (?, ?, ?)",
		{patternName, cornerCountX, cornerCountY});

	Pattern *pattern = new Pattern(m_db, patId);
	m_patterns.insert(pattern);
	emit patternAdded(pattern);

	return pattern;
}

void Project::removePattern(Pattern *pattern)
{
	m_db->exec("DELETE FROM pattern WHERE id=?", {pattern->m_patternId});

	m_patterns.remove(pattern);
	emit patternRemoved(pattern);

	delete pattern;
}

}
