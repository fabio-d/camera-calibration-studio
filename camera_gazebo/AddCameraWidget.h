#pragma once

#include "camera_gazebo/GazeboConnection.h"

#include "common/AddCameraWidget.h"

namespace ccs::camera_gazebo
{

class Ui_AddCameraWidget;

class AddCameraWidget : public common::AddCameraWidget
{
	Q_OBJECT

	public:
		explicit AddCameraWidget(QWidget *parent = nullptr);
		~AddCameraWidget();

		QString defaultCameraName() const override;
		bool canAccept() const override;

		QJsonObject pluginData() override;
		QList<common::SensorStaticInfo> sensorsStaticInfo() override;

	private:
		void rescanTopics();
		void topicSelected();
		void frameReceived(const cv::Mat &frame);

		Ui_AddCameraWidget *m_ui;

		QSharedPointer<GazeboConnection> m_conn;
		bool m_validTopic;
		QSharedPointer<GazeboSubscription> m_sub;

		int m_width, m_height;
};

}
