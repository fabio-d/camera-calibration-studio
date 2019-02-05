#pragma once

#include "common/LiveCapture.h"
#include "common/Project.h"
#include "common/Shot.h"

#include <QJsonObject>
#include <QSet>

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

		Project *project() const;

		const QString &name() const;
		const QList<Sensor*> sensors() const;

		CameraStaticInfo staticInfo() const;

		bool supportsLiveCapture() const;
		bool isLiveCaptureRunning() const;
		bool startLiveCapture();
		void stopLiveCapture();
		QList<BaseLiveCaptureParameter*> liveCaptureParameterList() const;
		const QMap<const Sensor*, cv::Mat> &lastCapturedFrame() const;

		Shot *addShot(const QString &name, const QJsonObject &captureParameters, const QMap<const Sensor*, cv::Mat> &frame);
		void removeShot(Shot *shot);
		const QSet<Shot*> &shots() const;

	signals:
		void liveCaptureRunningChanged();
		void liveCaptureParameterListChanged();
		void capturedFrameChanged();

		void shotAdded(Shot *shot);
		void shotRemoved(Shot *shot);

	private:
		Camera(SqliteDatabase *db, Project *project, int cameraId);

		void onFrameCaptured(const QList<cv::Mat> &images);
		void liveCaptureDestroyed();

		SqliteDatabase *m_db;
		Project *m_project;
		int m_cameraId;
		QString m_name;
		QList<Sensor*> m_sensors;

		LiveCapture *m_liveCapture; // nullptr if not running
		QMap<const Sensor*, cv::Mat> m_lastCapturedFrame;

		QSet<Shot*> m_shots;

		// CameraStaticInfo
		QString m_pluginId;
		QJsonObject m_pluginData;
};

}
