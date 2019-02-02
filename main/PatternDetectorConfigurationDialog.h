#pragma once

#include "main/PatternDetector.h"

#include "common/Pattern.h"
#include "common/Sensor.h"

#include <QDialog>
#include <QListWidgetItem>

namespace ccs::main
{

class Ui_PatternDetectorConfigurationDialog;

class PatternDetectorConfigurationDialog : public QDialog
{
	Q_OBJECT

	public:
		PatternDetectorConfigurationDialog(const QList<common::Pattern*> &patterns, const QList<common::Sensor*> &sensors, QWidget *parent = nullptr);
		~PatternDetectorConfigurationDialog();

		PatternDetectorConfiguration configuration() const;

	private:
		Ui_PatternDetectorConfigurationDialog *m_ui;

		QList<common::Pattern*> m_patterns;
		QMap<QListWidgetItem*, common::Sensor*> m_sensors;
};

}
