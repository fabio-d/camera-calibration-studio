#pragma once

#include "common/Sensor.h"

#include <QDockWidget>
#include <QLocale>

namespace ccs::main
{

class Ui_CalibrationDataDockWidget;

class CalibrationDataDockWidget : public QDockWidget
{
	Q_OBJECT

	public:
		CalibrationDataDockWidget(QWidget *parent);
		~CalibrationDataDockWidget();

		void showSensor(common::Sensor *sensor);

	protected:
		void editingFinished();
		void dockAreaChanged(Qt::DockWidgetArea area);
		void updateLayout();

	private:
		Ui_CalibrationDataDockWidget *m_ui;

		QLocale m_localeC;
		Qt::DockWidgetArea m_currentDockArea;
		common::Sensor *m_sensor;
};

}
