#pragma once

#include "common/AddCameraWidget.h"

namespace ccs::camera_V4L2
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
		void rescanDevices();
		void deviceSelected();
		bool loadDeviceInfo(int fd);

		Ui_AddCameraWidget *m_ui;

		bool m_validDevice;
		QString m_devicePath;
		QByteArray m_cardString, m_busInfoString, m_driverString;
		QList<QSize> m_supportedResolutions;
};

}
