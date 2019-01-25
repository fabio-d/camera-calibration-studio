#include "common/Shot.h"

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
	QByteArray data = sensorDataAsByteArray(sensor);
	if (data.isEmpty())
		return cv::Mat();
	else
		return cv::Mat(sensor->staticInfo().height, sensor->staticInfo().width, CV_8UC3, (void*)data.constData()).clone();
}

QByteArray Shot::sensorDataAsByteArray(const Sensor *sensor) const
{
	for (const QSqlRecord &queryShot : m_db->exec("SELECT data FROM sensor_data WHERE shot_id=? AND sensor_id=?", {m_shotId, sensor->m_sensorId}))
		return queryShot.value(0).toByteArray();

	return QByteArray();
}

}
