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
		bool isLiveCaptureRunning() const;
		bool startLiveCapture();
		void stopLiveCapture();
		const QMap<const Sensor*, cv::Mat> &lastCapturedFrame() const;

	signals:
		void liveCaptureRunningChanged();
		void capturedFrameChanged();

	private:
		Camera(SqliteDatabase *db, int cameraId);

		void onFrameCaptured(const QList<cv::Mat> &images);
		void liveCaptureDestroyed();

		SqliteDatabase *m_db;
		int m_cameraId;
		QString m_name;
		QList<Sensor*> m_sensors;

		LiveCapture *m_liveCapture; // nullptr if not running
		QMap<const Sensor*, cv::Mat> m_lastCapturedFrame;

		// CameraStaticInfo
		QString m_pluginId;
		QJsonObject m_pluginData;
};

}
