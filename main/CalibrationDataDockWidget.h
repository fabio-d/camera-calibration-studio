#pragma once

#include "common/Pattern.h"
#include "common/Sensor.h"

#include <QDockWidget>
#include <QLineEdit>

namespace ccs::main
{

class Ui_CalibrationDataDockWidget;

class CalibrationDataDockWidget : public QDockWidget
{
	Q_OBJECT

	public:
		CalibrationDataDockWidget(QWidget *parent);
		~CalibrationDataDockWidget();

		void showNothing();
		void showSensor(common::Sensor *sensor);
		void showPattern(common::Pattern *pattern);

	protected:
		void applyValues();
		void dockAreaChanged(Qt::DockWidgetArea area);
		void updateLayout();

	private:
		Ui_CalibrationDataDockWidget *m_ui;

		Qt::DockWidgetArea m_currentDockArea;
		common::Sensor *m_sensor;
		common::Pattern *m_pattern;

		QMap<const common::CalibrationParameter::MetaInfo*, QLineEdit*> m_sensorFields;
};

}
