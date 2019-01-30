#pragma once

#include <QMutex>
#include <QSharedPointer>

#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>

#include <opencv2/core/core.hpp>

namespace ccs::camera_gazebo
{

class GazeboConnection
{

	public:
		~GazeboConnection();

		static QSharedPointer<GazeboConnection> connectToGazebo();

		QStringList imageStampedTopicList() const;

	private:
		GazeboConnection() = default;
};

class GazeboSubscription : public QObject
{
	Q_OBJECT

	public:
		GazeboSubscription(const QSharedPointer<GazeboConnection> &conn, const QString &topicName);
		~GazeboSubscription();

	signals:
		void frameReceived(const cv::Mat &frame);

	private slots:
		void frameEnqueued();

	private:
		void onMessage(ConstImageStampedPtr &msg);

		QSharedPointer<GazeboConnection> m_conn;
		gazebo::transport::NodePtr m_node;
		gazebo::transport::SubscriberPtr m_sub;

		QMutex m_enqueuedFrameMutex;
		cv::Mat m_enqueuedFrame;
};

}
