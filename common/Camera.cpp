#include "common/Camera.h"

#include "common/CameraPlugin.h"
#include "common/LiveCapture.h"

#include <QDebug>
#include <QJsonDocument>

namespace ccs::common
{

Camera::Camera(SqliteDatabase *db, int cameraId)
: m_db(db)
, m_cameraId(cameraId)
, m_liveCapture(nullptr)
{
	QSqlRecord queryCamera = m_db->execReturnOne("SELECT * FROM camera WHERE id=?", {cameraId});

	m_name = queryCamera.value("name").toString();
	m_pluginId = queryCamera.value("plugin").toString();
	m_pluginData = QJsonDocument::fromJson(queryCamera.value("plugin_data").toByteArray()).object();

	for (const QSqlRecord &querySensor : m_db->exec("SELECT id FROM sensor WHERE camera_id=?", {cameraId}))
		m_sensors.append(new Sensor(m_db, querySensor.value(0).toInt()));
}

Camera::~Camera()
{
	delete m_liveCapture;
	qDeleteAll(m_sensors);
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

}
