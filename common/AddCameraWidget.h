#pragma once

#include <QJsonObject>
#include <QWidget>

namespace ccs::common
{

struct SensorStaticInfo;

class AddCameraWidget : public QWidget
{
	Q_OBJECT

	public:
		explicit AddCameraWidget(QWidget *parent = nullptr);

		virtual QString defaultCameraName() const = 0;
		virtual bool canAccept() const = 0;

		virtual QJsonObject pluginData() = 0;
		virtual QList<SensorStaticInfo> sensorsStaticInfo() = 0;

	signals:
		void defaultCameraNameChanged();
		void canAcceptChanged();
};

}
