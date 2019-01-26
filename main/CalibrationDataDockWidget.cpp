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
		connect(le, &QLineEdit::textEdited, this, &CalibrationDataDockWidget::editingFinished);

		m_fields.insert(&m, le);
	}
}

CalibrationDataDockWidget::~CalibrationDataDockWidget()
{
	delete m_ui;
}

void CalibrationDataDockWidget::showSensor(common::Sensor *sensor)
{
	m_sensor = nullptr;

	if (sensor == nullptr)
	{
		m_ui->dockWidgetContents->setCurrentWidget(m_ui->noParametersPage);

		for (auto it = m_fields.begin(); it != m_fields.end(); ++it)
			it.value()->clear();
	}
	else
	{
		m_ui->dockWidgetContents->setCurrentWidget(m_ui->parametersPage);

		const common::CalibrationParameters &p = sensor->calibrationParameters();

		for (auto it = m_fields.begin(); it != m_fields.end(); ++it)
			it.value()->setText(it.key()->valueAsText(&p));

		m_sensor = sensor;
	}
}

void CalibrationDataDockWidget::editingFinished()
{
	common::CalibrationParameters p = m_sensor->calibrationParameters();

	for (auto it = m_fields.begin(); it != m_fields.end(); ++it)
		it.key()->setValueFromText(&p, it.value()->text());

	m_sensor->setCalibrationParameters(p);
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
		m_ui->parametersPage->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_ui->parametersPage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_ui->parametersPage->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
		m_ui->parametersPage->setMinimumSize(
			m_ui->parametersLayout->minimumSize().width()
				+ m_ui->parametersPage->verticalScrollBar()->width()
				+ m.left() + m.right(),
			0);
	}
	else
	{
		m_ui->parametersLayout->setDirection(QBoxLayout::LeftToRight);
		m_ui->parametersPage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		m_ui->parametersPage->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
		m_ui->parametersPage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
		m_ui->parametersPage->setMinimumSize(
			0,
			m_ui->parametersLayout->minimumSize().height()
				+ m_ui->parametersPage->horizontalScrollBar()->height()
				+ m.top() + m.bottom());
	}
}

}
