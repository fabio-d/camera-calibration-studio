#include "main/CalibrationDataDockWidget.h"

#include <QScrollBar>

#include <math.h>

#include "ui_CalibrationDataDockWidget.h"

namespace ccs::main
{

static const QVector<QPair<double common::Sensor::CalibrationParameters::*, QLineEdit* Ui_CalibrationDataDockWidget::*>> fields
{
	{ &common::Sensor::CalibrationParameters::fx, &Ui_CalibrationDataDockWidget::fxLineEdit },
	{ &common::Sensor::CalibrationParameters::fy, &Ui_CalibrationDataDockWidget::fyLineEdit },
	{ &common::Sensor::CalibrationParameters::cx, &Ui_CalibrationDataDockWidget::cxLineEdit },
	{ &common::Sensor::CalibrationParameters::cy, &Ui_CalibrationDataDockWidget::cyLineEdit },
	{ &common::Sensor::CalibrationParameters::p1, &Ui_CalibrationDataDockWidget::p1LineEdit },
	{ &common::Sensor::CalibrationParameters::p2, &Ui_CalibrationDataDockWidget::p2LineEdit },
	{ &common::Sensor::CalibrationParameters::k1, &Ui_CalibrationDataDockWidget::k1LineEdit },
	{ &common::Sensor::CalibrationParameters::k2, &Ui_CalibrationDataDockWidget::k2LineEdit },
	{ &common::Sensor::CalibrationParameters::k3, &Ui_CalibrationDataDockWidget::k3LineEdit },
	{ &common::Sensor::CalibrationParameters::k4, &Ui_CalibrationDataDockWidget::k4LineEdit },
	{ &common::Sensor::CalibrationParameters::k5, &Ui_CalibrationDataDockWidget::k5LineEdit },
	{ &common::Sensor::CalibrationParameters::k6, &Ui_CalibrationDataDockWidget::k6LineEdit }
};

CalibrationDataDockWidget::CalibrationDataDockWidget(QWidget *parent)
: QDockWidget(parent)
, m_ui(new Ui_CalibrationDataDockWidget)
, m_localeC("C")
, m_currentDockArea(Qt::BottomDockWidgetArea)
, m_sensor(nullptr)
{
	m_ui->setupUi(this);

	m_localeC.setNumberOptions(QLocale::RejectGroupSeparator | QLocale::RejectLeadingZeroInExponent);

	connect(this, &QDockWidget::dockLocationChanged, this, &CalibrationDataDockWidget::dockAreaChanged);
	connect(this, &QDockWidget::topLevelChanged, this, &CalibrationDataDockWidget::updateLayout);

	QDoubleValidator *validator = new QDoubleValidator(this);
	validator->setLocale(m_localeC);

	for (const auto &it : fields)
	{
		QLineEdit *le = m_ui->*it.second;
		le->setValidator(validator);

		connect(le, &QLineEdit::textEdited, this, &CalibrationDataDockWidget::editingFinished);
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

		for (const auto &it : fields)
			(m_ui->*it.second)->clear();
	}
	else
	{
		m_ui->dockWidgetContents->setCurrentWidget(m_ui->parametersPage);

		common::Sensor::CalibrationParameters p = sensor->calibrationParameters();
		for (const auto &it : fields)
		{
			double v = p.*it.first;
			if (std::isnan(v))
				(m_ui->*it.second)->clear();
			else
				(m_ui->*it.second)->setText(QString::number(v));
		}

		m_sensor = sensor;
	}
}

void CalibrationDataDockWidget::editingFinished()
{
	common::Sensor::CalibrationParameters p = m_sensor->calibrationParameters();

	for (const auto &it : fields)
	{
		QLineEdit *le = m_ui->*it.second;
		if (sender() == le)
		{
			bool ok;
			double v = le->text().toDouble(&ok);

			p.*it.first = ok ? v : NAN;
			break;
		}
	}

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
