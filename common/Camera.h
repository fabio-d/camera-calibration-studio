#pragma once

#include "common/Sensor.h"

#include <QJsonObject>

namespace ccs::common
{

class LiveCapture;

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

		bool supportsLiveCapture() const;
		bool startLiveCapture();
		void stopLiveCapture();
		LiveCapture *liveCapture() const; // nullptr if not running

	signals:
		void liveCaptureChanged();

	private:
		Camera(SqliteDatabase *db, int cameraId);

		void liveCaptureDestroyed();

		SqliteDatabase *m_db;
		int m_cameraId;
		QString m_name;
		QList<Sensor*> m_sensors;

		LiveCapture *m_liveCapture;

		// CameraStaticInfo
		QString m_pluginId;
		QJsonObject m_pluginData;
};

}
