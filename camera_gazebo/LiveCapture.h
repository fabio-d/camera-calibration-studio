#pragma once

#include "camera_gazebo/GazeboConnection.h"

#include "common/LiveCapture.h"

namespace ccs::camera_gazebo
{

class LiveCapture : public common::LiveCapture
{
	Q_OBJECT

	public:
		LiveCapture(const QJsonObject &pluginData);
		~LiveCapture();

	private:
		void frameCallback(const cv::Mat &frame);

		QSharedPointer<GazeboConnection> m_conn;
		QSharedPointer<GazeboSubscription> m_sub;
		cv::Size m_expectedSize;
};

}
