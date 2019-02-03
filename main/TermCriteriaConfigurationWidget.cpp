#include "main/TermCriteriaConfigurationWidget.h"

#include "ui_TermCriteriaConfigurationWidget.h"

namespace ccs::main
{

TermCriteriaConfigurationWidget::TermCriteriaConfigurationWidget(QWidget *parent)
: QGroupBox(parent)
, m_ui(new Ui_TermCriteriaConfigurationWidget)
{
	m_ui->setupUi(this);

	QDoubleValidator *v = new QDoubleValidator(this);
	v->setBottom(0);
	m_ui->epsilonLineEdit->setValidator(v);
}

TermCriteriaConfigurationWidget::~TermCriteriaConfigurationWidget()
{
	delete m_ui;
}

void TermCriteriaConfigurationWidget::setValue(const cv::TermCriteria &value)
{
	if (value.type == cv::TermCriteria::EPS)
	{
		m_ui->epsilonRadioButton->setChecked(true);
	}
	else if (value.type == cv::TermCriteria::MAX_ITER)
	{
		m_ui->maxIterationsRadioButton->setChecked(true);
	}
	else
	{
		assert(value.type == cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS);

		m_ui->bothRadioButton->setChecked(true);
	}

	if ((value.type & cv::TermCriteria::EPS))
	{
		if (value.epsilon == DBL_EPSILON)
			m_ui->epsilonLineEdit->clear();
		else
			m_ui->epsilonLineEdit->setText(QString::number(value.epsilon));
	}

	if ((value.type & cv::TermCriteria::MAX_ITER))
		m_ui->maxIterationsSpinBox->setValue(value.maxCount);
}

cv::TermCriteria TermCriteriaConfigurationWidget::value() const
{
	cv::TermCriteria r;

	if (m_ui->epsilonRadioButton->isChecked())
		r.type = cv::TermCriteria::EPS;
	else if (m_ui->maxIterationsRadioButton->isChecked())
		r.type = cv::TermCriteria::MAX_ITER;
	else
		r.type = cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS;

	bool ok;
	r.epsilon = m_ui->epsilonLineEdit->text().toDouble(&ok);
	if (!ok)
		r.epsilon = DBL_EPSILON;

	r.maxCount = m_ui->maxIterationsSpinBox->value();

	return r;
}

}
