#pragma once

#include "common/Sensor.h"

#include <QJsonObject>

namespace ccs::common
{

class Camera;
class Pattern;

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

		void setPatternData(const Sensor *sensor, const ccs::common::Pattern *pattern, const std::vector<cv::Point2f> &coordinates);
		void unsetPatternData(const Sensor *sensor);
		QPair<const ccs::common::Pattern*, std::vector<cv::Point2f>> patternData(const Sensor *sensor) const;

	private:
		Shot(SqliteDatabase *db, Camera *camera, int shotId);

		bool maybeSensorData(const Sensor *sensor, QByteArray *out_data) const;
		bool maybePatternData(const Sensor *sensor, int *out_patternId, QString *out_coordinates) const;

		SqliteDatabase *m_db;
		Camera *m_camera;
		int m_shotId;
		QString m_name;
		QJsonObject m_captureParameters;
};

}
