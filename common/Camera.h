#pragma once

#include "common/Sensor.h"

#include <QJsonObject>

namespace ccs::common
{

struct CameraStaticInfo
{
	QString pluginId;
	QJsonObject pluginData;
	QList<SensorStaticInfo> sensors;
};

class Camera : public QObject
{
	Q_OBJECT

	friend class Project;

	public:
		~Camera();

		const QString &name() const;
		const QList<Sensor*> sensors() const;

		CameraStaticInfo staticInfo() const;

	private:
		Camera(SqliteDatabase *db, int cameraId);

		SqliteDatabase *m_db;
		int m_cameraId;
		QString m_name;
		QList<Sensor*> m_sensors;

		// CameraStaticInfo
		QString m_pluginId;
		QJsonObject m_pluginData;
};

}
