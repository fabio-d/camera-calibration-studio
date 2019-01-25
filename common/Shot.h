#pragma once

#include "common/Sensor.h"

#include <QJsonObject>

namespace ccs::common
{

class Camera;

class Shot : public QObject
{
	Q_OBJECT

	friend class Camera;

	public:
		~Shot();

		Camera *camera() const;
		const QString &name() const;

		const QJsonObject captureParameters() const;
		cv::Mat sensorData(const Sensor *sensor) const;

	private:
		Shot(SqliteDatabase *db, Camera *camera, int shotId);

		QByteArray sensorDataAsByteArray(const Sensor *sensor) const;

		SqliteDatabase *m_db;
		Camera *m_camera;
		int m_shotId;
		QString m_name;
		QJsonObject m_captureParameters;
};

}
