#include "main/CalibrationDataDockWidget.h"

#include <QScrollBar>

#include <math.h>

#include "ui_CalibrationDataDockWidget.h"

namespace ccs::main
{

CalibrationDataDockWidget::CalibrationDataDockWidget(QWidget *parent)
: QDockWidget(parent)
, m_ui(new Ui_CalibrationDataDockWidget)
, m_currentDockArea(Qt::BottomDockWidgetArea)
, m_sensor(nullptr)
, m_pattern(nullptr)
{
	m_ui->setupUi(this);

	//m_localeC.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::RejectLeadingZeroInExponent);

	connect(this, &QDockWidget::dockLocationChanged, this, &CalibrationDataDockWidget::dockAreaChanged);
	connect(this, &QDockWidget::topLevelChanged, this, &CalibrationDataDockWidget::updateLayout);

	for (const common::CalibrationParameter::MetaInfo &m : common::CalibrationParameters::allFields())
	{
		QString fieldName = QString("%1LineEdit").arg(m.name());
		QLineEdit *le = findChild<QLineEdit*>(fieldName);
		if (le == nullptr)
			qFatal("Field \"%s\" not found", fieldName.toLatin1().constData());

		le->setValidator(m.createValidator(this));
		connect(le, &QLineEdit::textEdited, this, &CalibrationDataDockWidget::applyValues);

		m_sensorFields.insert(&m, le);
	}

	connect(m_ui->cornerDistanceXDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &CalibrationDataDockWidget::applyValues);
	connect(m_ui->cornerDistanceYDoubleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &CalibrationDataDockWidget::applyValues);
}

CalibrationDataDockWidget::~CalibrationDataDockWidget()
{
	delete m_ui;
}

void CalibrationDataDockWidget::showNothing()
{
	showSensor(nullptr);
	showPattern(nullptr);

	m_ui->dockWidgetContents->setCurrentWidget(m_ui->noParametersPage);
}

void CalibrationDataDockWidget::showSensor(common::Sensor *sensor)
{
	if (m_sensor == sensor)
		return;

	m_sensor = nullptr;
	showPattern(nullptr);

	if (sensor == nullptr)
	{
		for (auto it = m_sensorFields.begin(); it != m_sensorFields.end(); ++it)
			it.value()->clear();
	}
	else
	{
		m_ui->dockWidgetContents->setCurrentWidget(m_ui->sensorParametersPage);

		const common::CalibrationParameters &p = sensor->calibrationParameters();

		for (auto it = m_sensorFields.begin(); it != m_sensorFields.end(); ++it)
			it.value()->setText(it.key()->valueAsText(&p));

		m_sensor = sensor;
	}
}

void CalibrationDataDockWidget::showPattern(common::Pattern *pattern)
{
	if (m_pattern == pattern)
		return;

	m_pattern = nullptr;
	showSensor(nullptr);

	if (pattern == nullptr)
	{
		m_ui->cornerDistanceXDoubleSpinBox->clear();
		m_ui->cornerDistanceYDoubleSpinBox->clear();
	}
	else
	{
		m_ui->dockWidgetContents->setCurrentWidget(m_ui->patternParametersPage);

		m_ui->cornerDistanceXDoubleSpinBox->setValue(pattern->cornerDistanceX());
		m_ui->cornerDistanceYDoubleSpinBox->setValue(pattern->cornerDistanceY());

		m_pattern = pattern;
	}
}

void CalibrationDataDockWidget::applyValues()
{
	if (m_sensor != nullptr)
	{
		common::CalibrationParameters p = m_sensor->calibrationParameters();

		for (auto it = m_sensorFields.begin(); it != m_sensorFields.end(); ++it)
			it.key()->setValueFromText(&p, it.value()->text());

		m_sensor->setCalibrationParameters(p);
	}
	else if (m_pattern != nullptr)
	{
		m_pattern->setCornerDistanceX(m_ui->cornerDistanceXDoubleSpinBox->value());
		m_pattern->setCornerDistanceY(m_ui->cornerDistanceYDoubleSpinBox->value());
	}
}

void CalibrationDataDockWidget::dockAreaChanged(Qt::DockWidgetArea area)
{
	m_currentDockArea = area;
	updateLayout();
}

void CalibrationDataDockWidget::updateLayout()
{
	QMargins m = m_ui->parametersLayout->contentsMargins();

	if (m_currentDockArea == Qt::LeftDockWidgetArea || m_currentDockArea == Qt::RightDockWidgetArea || isFloating())
	{
		m_ui->parametersLayout->setDirection(QBoxLayout::TopToBottom);
		m_ui->sensorParametersPage->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_ui->sensorParametersPage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_ui->sensorParametersPage->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
		m_ui->sensorParametersPage->setMinimumSize(
			m_ui->parametersLayout->minimumSize().width()
				+ m_ui->sensorParametersPage->verticalScrollBar()->width()
				+ m.left() + m.right(),
			0);
	}
	else
	{
		m_ui->parametersLayout->setDirection(QBoxLayout::LeftToRight);
		m_ui->sensorParametersPage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_ui->sensorParametersPage->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_ui->sensorParametersPage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
		m_ui->sensorParametersPage->setMinimumSize(
			0,
			m_ui->parametersLayout->minimumSize().height()
				+ m_ui->sensorParametersPage->horizontalScrollBar()->height()
				+ m.top() + m.bottom());
	}
}

}
