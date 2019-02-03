#include "main/PatternDetectorConfigurationDialog.h"

#include "common/Camera.h"

#include "ui_PatternDetectorConfigurationDialog.h"

namespace ccs::main
{

PatternDetectorConfigurationDialog::PatternDetectorConfigurationDialog(const QList<common::Pattern*> &patterns, const QList<common::Sensor*> &sensors, QWidget *parent)
: QDialog(parent)
, m_ui(new Ui_PatternDetectorConfigurationDialog)
, m_patterns(patterns)
{
	m_ui->setupUi(this);

	for (common::Pattern *p : patterns)
		m_ui->patternComboBox->addItem(p->name());

	for (common::Sensor *s : sensors)
	{
		QListWidgetItem *it = new QListWidgetItem(
			s->camera()->name() + " - " + s->staticInfo().name,
			m_ui->sensorListWidget);
		it->setCheckState(Qt::Checked);
		m_sensors.insert(it, s);
	}

	m_ui->policyComboBox->addItem("Skip images where a pattern is already present",
		QVariant(PatternDetectorConfiguration::NeverOverwrite));
	m_ui->policyComboBox->addItem("Overwrite existing pattern only if a new one is found",
		QVariant(PatternDetectorConfiguration::OverwriteIfFound));
	m_ui->policyComboBox->addItem("Replace existing pattern, or delete if none is found",
		QVariant(PatternDetectorConfiguration::AlwaysOverwrite));
}

PatternDetectorConfigurationDialog::~PatternDetectorConfigurationDialog()
{
	delete m_ui;
}

PatternDetectorConfiguration PatternDetectorConfigurationDialog::configuration() const
{
	PatternDetectorConfiguration r;

	r.pattern = m_patterns[m_ui->patternComboBox->currentIndex()];
	r.normalizeImage = m_ui->normalizeImageCheckBox->isChecked();
	r.adaptiveThreshold = m_ui->adaptiveThresholdCheckBox->isChecked();
	r.filterQuads = m_ui->filterQuadsCheckBox->isChecked();
	r.fastCheck = m_ui->fastCheckCheckBox->isChecked();
	r.cornerRefinerConfig = m_ui->cornerRefinerConfigWidget->configuration();
	r.policy = (PatternDetectorConfiguration::OverwritePolicy)m_ui->policyComboBox->currentData().toInt();

	// Populate r.affectedSensors
	for (auto it = m_sensors.begin(); it != m_sensors.end(); ++it)
	{
		if (it.key()->checkState() == Qt::Checked)
			r.affectedSensors.insert(it.value());
	}

	return r;
}

}
