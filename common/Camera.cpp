#include "common/Camera.h"

#include "common/CameraPlugin.h"
#include "common/LiveCapture.h"

#include <QDebug>
#include <QJsonDocument>

namespace ccs::common
{

Camera::Camera(SqliteDatabase *db, Project *project, int cameraId)
: m_db(db)
, m_project(project)
, m_cameraId(cameraId)
, m_liveCapture(nullptr)
{
	QSqlRecord queryCamera = m_db->execReturnOne("SELECT * FROM camera WHERE id=?", {cameraId});

	m_name = queryCamera.value("name").toString();
	m_pluginId = queryCamera.value("plugin").toString();
	m_pluginData = QJsonDocument::fromJson(queryCamera.value("plugin_data").toByteArray()).object();

	for (const QSqlRecord &querySensor : m_db->exec("SELECT id FROM sensor WHERE camera_id=?", {cameraId}))
		m_sensors.append(new Sensor(m_db, this, querySensor.value(0).toInt()));

	for (const QSqlRecord &queryShot : m_db->exec("SELECT id FROM shot WHERE camera_id=?", {cameraId}))
		m_shots.insert(new Shot(m_db, this, queryShot.value(0).toInt()));
}

Camera::~Camera()
{
	delete m_liveCapture;
	qDeleteAll(m_shots);
	qDeleteAll(m_sensors);
}

Project *Camera::project() const
{
	return m_project;
}

const QString &Camera::name() const
{
	return m_name;
}

const QList<Sensor*> Camera::sensors() const
{
	return m_sensors;
}

CameraStaticInfo Camera::staticInfo() const
{
	CameraStaticInfo r;
	r.pluginId = m_pluginId;
	r.pluginData = m_pluginData;
	r.sensors = {};
	return r;
}

bool Camera::supportsLiveCapture() const
{
	CameraPlugin *plugin = CameraPlugin::pluginInstance(m_pluginId);
	if (plugin != nullptr)
		return plugin->haveLiveCaptureSupport();
	else
		return false;
}

bool Camera::isLiveCaptureRunning() const
{
	return m_liveCapture != nullptr;
}

bool Camera::startLiveCapture()
{
	if (m_liveCapture != nullptr)
		return false; // Live capture is already running

	CameraPlugin *plugin = CameraPlugin::pluginInstance(m_pluginId);
	if (plugin == nullptr)
	{
		qCritical() << "Plugin" << m_pluginId << "is not available";
		return false;
	}
	else
	{
		m_liveCapture = plugin->startLiveCapture(m_pluginData);
		if (m_liveCapture == nullptr)
		{
			qCritical() << "Plugin" << m_pluginId << "failed to start:" << m_pluginData;
			return false;
		}
	}

	// The LiveCapture will either be explicitly deleted by the user to stop
	// the live capture session (see stopLiveCapture method), or it will
	// self-delete in case of camera acquisition errors.
	connect(m_liveCapture, &QObject::destroyed, this, &Camera::liveCaptureDestroyed);

	// New frame callback
	connect(m_liveCapture, &LiveCapture::frameCaptured, this, &Camera::onFrameCaptured);

	emit liveCaptureRunningChanged();
	return true;
}

void Camera::stopLiveCapture()
{
	// m_liveCapture will be reset to nullptr by slot liveCaptureDestroyed
	delete m_liveCapture;
}

const QMap<const Sensor*, cv::Mat> &Camera::lastCapturedFrame() const
{
	return m_lastCapturedFrame;
}

void Camera::onFrameCaptured(const QList<cv::Mat> &images)
{
	assert(images.size() == m_sensors.size());

	m_lastCapturedFrame.clear();
	for (int i = 0; i < m_sensors.size(); i++)
		m_lastCapturedFrame.insert(m_sensors[i], images[i]);

	emit capturedFrameChanged();
}

void Camera::liveCaptureDestroyed()
{
	m_lastCapturedFrame.clear();
	m_liveCapture = nullptr;

	emit liveCaptureRunningChanged();
	emit capturedFrameChanged();
}

Shot *Camera::addShot(const QString &name, const QJsonObject &captureParameters, const QMap<const Sensor*, cv::Mat> &frame)
{
	int shotId = m_db->execInsertId("INSERT INTO shot(name, capture_parameters, camera_id) VALUES (?, ?, ?)",
		 {name, QJsonDocument(captureParameters).toJson(), m_cameraId});

	for (QMap<const Sensor*, cv::Mat>::const_iterator it = frame.begin(); it != frame.end(); ++it)
	{
		const cv::Mat &m = it.value();

		m_db->exec("INSERT INTO sensor_data(shot_id, sensor_id, camera_id, data) VALUES (?, ?, ?, ?)",
			{shotId, it.key()->m_sensorId, m_cameraId, QByteArray::fromRawData((const char*)m.data, m.rows * m.cols * 3)});
	}

	Shot *shot = new Shot(m_db, this, shotId);
	m_shots.insert(shot);
	emit shotAdded(shot);

	return shot;
}

void Camera::removeShot(Shot *shot)
{
	m_shots.remove(shot);
	emit shotRemoved(shot);

	m_db->exec("DELETE FROM shot WHERE id=?", {shot->m_shotId});

	delete shot;
}

const QSet<Shot*> &Camera::shots() const
{
	return m_shots;
}

}
