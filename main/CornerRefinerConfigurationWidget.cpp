#include "main/CornerRefinerConfigurationWidget.h"

#include "ui_CornerRefinerConfigurationWidget.h"

namespace ccs::main
{

CornerRefinerConfigurationWidget::CornerRefinerConfigurationWidget(QWidget *parent)
: QGroupBox(parent)
, m_ui(new Ui_CornerRefinerConfigurationWidget)
{
	m_ui->setupUi(this);

	m_ui->termCriteriaConfigWidget->setValue(cv::TermCriteria(
		cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 30, 0.1));
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
	r.termCriteria = m_ui->termCriteriaConfigWidget->value();

	return r;
}

}
