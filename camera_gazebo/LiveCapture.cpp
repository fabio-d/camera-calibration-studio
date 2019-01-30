#include "camera_gazebo/LiveCapture.h"

#include <QDebug>
#include <QJsonObject>

namespace ccs::camera_gazebo
{

LiveCapture::LiveCapture(const QJsonObject &pluginData)
: m_conn(GazeboConnection::connectToGazebo())
{
	if (m_conn.isNull())
	{
		qCritical() << "Failed to connect to Gazebo";
		deleteLater();
		return;
	}

	QString topic = pluginData.value("topic").toString();
	m_expectedSize.width = pluginData.value("width").toInt();
	m_expectedSize.height = pluginData.value("height").toInt();

	qCritical() << "STARTED";

	GazeboSubscription *sub = new GazeboSubscription(m_conn, topic);
	connect(sub, &GazeboSubscription::frameReceived, this, &LiveCapture::frameCallback);
	m_sub.reset(sub);
}

LiveCapture::~LiveCapture()
{
	qCritical() << "STOPPING";
}

void LiveCapture::frameCallback(const cv::Mat &frame)
{
	if (frame.size() != m_expectedSize)
	{
		qCritical() << "Image size mismatch";
		deleteLater();
		return;
	}

	emit frameCaptured(QList<cv::Mat>() << frame);
}

}
