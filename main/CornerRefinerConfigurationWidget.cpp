#include "main/CornerRefinerConfigurationWidget.h"

#include "ui_CornerRefinerConfigurationWidget.h"

namespace ccs::main
{

CornerRefinerConfigurationWidget::CornerRefinerConfigurationWidget(QWidget *parent)
: QGroupBox(parent)
, m_ui(new Ui_CornerRefinerConfigurationWidget)
{
	m_ui->setupUi(this);

	QDoubleValidator *v = new QDoubleValidator(this);
	v->setBottom(0);
	m_ui->epsilonLineEdit->setValidator(v);
}

CornerRefinerConfigurationWidget::~CornerRefinerConfigurationWidget()
{
	delete m_ui;
}

CornerRefinerConfiguration CornerRefinerConfigurationWidget::configuration() const
{
	CornerRefinerConfiguration r;

	r.enable = isChecked();
	r.winSize.width = m_ui->windowSizeWidthSpinBox->value();
	r.winSize.height = m_ui->windowSizeHeightSpinBox->value();
	r.zeroZone.width = m_ui->zeroZoneWidthSpinBox->value();
	r.zeroZone.height = m_ui->zeroZoneHeightSpinBox->value();

	if (m_ui->epsilonRadioButton->isChecked())
		r.termCriteria.type = cv::TermCriteria::EPS;
	else if (m_ui->maxIterationsRadioButton->isChecked())
		r.termCriteria.type = cv::TermCriteria::MAX_ITER;
	else
		r.termCriteria.type = cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS;

	bool ok;
	r.termCriteria.epsilon = m_ui->epsilonLineEdit->text().toDouble(&ok);
	if (!ok)
		r.termCriteria.epsilon = DBL_EPSILON;

	r.termCriteria.maxCount = m_ui->maxIterationsSpinBox->value();

	return r;
}

}
