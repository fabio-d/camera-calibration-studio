#include "common/Shot.h"

#include "common/Camera.h"
#include "common/Pattern.h"

#include <QDebug>
#include <QJsonDocument>

namespace ccs::common
{

Shot::Shot(SqliteDatabase *db, Camera *camera, int shotId)
: m_db(db)
, m_camera(camera)
, m_shotId(shotId)
{
	QSqlRecord queryShot = m_db->execReturnOne("SELECT * FROM shot WHERE id=?", {shotId});

	m_name = queryShot.value("name").toString();
	m_captureParameters = QJsonDocument::fromJson(queryShot.value("capture_parameters").toByteArray()).object();
}

Shot::~Shot()
{
}

Camera *Shot::camera() const
{
	return m_camera;
}

const QString &Shot::name() const
{
	return m_name;
}

const QJsonObject Shot::captureParameters() const
{
	return m_captureParameters;
}

cv::Mat Shot::sensorData(const Sensor *sensor) const
{
	QByteArray data;

	if (!maybeSensorData(sensor, &data))
		return cv::Mat();

	return cv::Mat(sensor->staticInfo().height, sensor->staticInfo().width, CV_8UC3, (void*)data.constData()).clone();
}

bool Shot::maybeSensorData(const Sensor *sensor, QByteArray *out_data) const
{
	for (const QSqlRecord &queryShot : m_db->exec("SELECT data FROM sensor_data WHERE shot_id=? AND sensor_id=?", {m_shotId, sensor->m_sensorId}))
	{
		*out_data = queryShot.value(0).toByteArray();
		return true;
	}

	return false;
}

void Shot::setPatternData(const Sensor *sensor, const ccs::common::Pattern *pattern, const std::vector<cv::Point2f> &coordinates)
{
	QStringList coordList;
	for (const cv::Point2f &p : coordinates)
		coordList << QString::number(p.x) << QString::number(p.y);

	m_db->exec("UPDATE sensor_data SET pattern_id=?, pattern_coordinates=? WHERE shot_id=? AND sensor_id=?",
		{pattern->m_patternId, coordList.join(","), m_shotId, sensor->m_sensorId});
}

void Shot::unsetPatternData(const Sensor *sensor)
{
	m_db->exec("UPDATE sensor_data SET pattern_id=NULL WHERE shot_id=? AND sensor_id=?", {m_shotId, sensor->m_sensorId});
}

QPair<const ccs::common::Pattern*, std::vector<cv::Point2f>> Shot::patternData(const Sensor *sensor) const
{
	QString coordListJoined;
	int patternId;

	if (!maybePatternData(sensor, &patternId, &coordListJoined))
		return { nullptr, {} };

	ccs::common::Project *project = m_camera->project();

	std::vector<cv::Point2f> coordinates;
	QStringList coordList = coordListJoined.split(",");
	for (size_t i = 0; i < coordList.count() / 2; i++)
		coordinates.emplace_back(coordList[i*2].toFloat(), coordList[i*2+1].toFloat());

	return { project->m_patterns.value(patternId), coordinates };
}

bool Shot::maybePatternData(const Sensor *sensor, int *out_patternId, QString *out_coordinates) const
{
	for (const QSqlRecord &queryShot : m_db->exec("SELECT pattern_id, pattern_coordinates FROM sensor_data WHERE shot_id=? AND sensor_id=?", {m_shotId, sensor->m_sensorId}))
	{
		if (queryShot.isNull(0))
			return false;

		*out_patternId = queryShot.value(0).toInt();
		*out_coordinates = queryShot.value(1).toString();
		return true;
	}

	return false;
}

}
