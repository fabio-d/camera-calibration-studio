#include "main/CaptureParameterWidget.h"

#include "ui_CaptureParameterWidget.h"

namespace ccs::main
{

CaptureParameterWidget::CaptureParameterWidget(common::BaseLiveCaptureParameter *param, QWidget *parent)
: QWidget(parent)
, m_ui(new Ui_CaptureParameterWidget)
, m_param(param)
, m_loading(false)
{
	m_ui->setupUi(this);

	m_ui->nameLabel->setText(m_param->name());

	connect(m_ui->integerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &CaptureParameterWidget::changed);
	connect(m_ui->singleChoiceComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &CaptureParameterWidget::changed);
	connect(m_ui->restoreDefaultButton, &QToolButton::clicked,
		this, &CaptureParameterWidget::restore);

	connect(m_param, &common::BaseLiveCaptureParameter::somethingChanged,
		this, &CaptureParameterWidget::setup);
	setup();
}

CaptureParameterWidget::~CaptureParameterWidget()
{
	delete m_ui;
}

void CaptureParameterWidget::setup()
{
	bool canBeRead = m_param->canBeRead();
	bool canBeWritten = m_param->canBeWritten();

	if (!canBeRead && !canBeWritten)
	{
		m_ui->stackedWidget->setCurrentWidget(m_ui->inactiveLabel);
		m_ui->restoreDefaultButton->setEnabled(false);
		return;
	}

	QString defaultAsText;
	m_loading = true;

	switch (m_param->type())
	{
		case common::BaseLiveCaptureParameter::Integer:
		{
			common::IntegerLiveCaptureParameter *p =
				static_cast<common::IntegerLiveCaptureParameter*>(m_param);

			m_ui->stackedWidget->setCurrentWidget(m_ui->integerWidget);

			m_ui->integerHorizontalSlider->setRange(p->minimumValue(), p->maximumValue());
			m_ui->integerSpinBox->setRange(p->minimumValue(), p->maximumValue());

			m_ui->integerHorizontalSlider->setSingleStep(p->step());
			m_ui->integerSpinBox->setSingleStep(p->step());

			int defaultValue = p->defaultValue();
			int value = canBeRead ? p->value() : defaultValue;
			m_ui->integerHorizontalSlider->setValue(value);
			m_ui->integerSpinBox->setValue(value);

			m_ui->integerHorizontalSlider->setEnabled(canBeWritten);
			m_ui->integerSpinBox->setReadOnly(!canBeWritten);

			m_ui->integerHorizontalSlider->setVisible(p->suggestSlider());
			defaultAsText = QString::number(defaultValue);

			break;
		}
		case common::BaseLiveCaptureParameter::SingleChoice:
		{
			common::SingleChoiceLiveCaptureParameter *p =
				static_cast<common::SingleChoiceLiveCaptureParameter*>(m_param);

			QStringList choices = p->choices();

			m_ui->stackedWidget->setCurrentWidget(m_ui->singleChoiceComboBox);

			m_ui->singleChoiceComboBox->clear();
			m_ui->singleChoiceComboBox->addItems(choices);

			int defaultChoice = p->defaultChoice();
			int choice = canBeRead ? p->currentChoice() : defaultChoice;
			m_ui->singleChoiceComboBox->setCurrentIndex(choice);

			m_ui->singleChoiceComboBox->setEnabled(canBeWritten);
			defaultAsText = choices[defaultChoice];

			break;
		}
	}

	m_ui->restoreDefaultButton->setEnabled(canBeWritten);
	m_ui->restoreDefaultButton->setToolTip(QString("%1 (%2)")
		.arg(m_ui->restoreDefaultButton->text()).arg(defaultAsText));

	m_loading = false;
}

void CaptureParameterWidget::changed()
{
	if (m_loading || !m_param->canBeWritten())
		return;

	switch (m_param->type())
	{
		case common::BaseLiveCaptureParameter::Integer:
		{
			common::IntegerLiveCaptureParameter *p =
				static_cast<common::IntegerLiveCaptureParameter*>(m_param);

			p->setValue(m_ui->integerSpinBox->value());

			break;
		}
		case common::BaseLiveCaptureParameter::SingleChoice:
		{
			common::SingleChoiceLiveCaptureParameter *p =
				static_cast<common::SingleChoiceLiveCaptureParameter*>(m_param);

			p->setCurrentChoice(m_ui->singleChoiceComboBox->currentIndex());

			break;
		}
	}
}

void CaptureParameterWidget::restore()
{
	switch (m_param->type())
	{
		case common::BaseLiveCaptureParameter::Integer:
		{
			common::IntegerLiveCaptureParameter *p =
				static_cast<common::IntegerLiveCaptureParameter*>(m_param);

			m_ui->integerSpinBox->setValue(p->defaultValue());

			break;
		}
		case common::BaseLiveCaptureParameter::SingleChoice:
		{
			common::SingleChoiceLiveCaptureParameter *p =
				static_cast<common::SingleChoiceLiveCaptureParameter*>(m_param);

			m_ui->singleChoiceComboBox->setCurrentIndex(p->defaultChoice());

			break;
		}
	}
}

}
