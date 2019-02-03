#include "main/CalibrationDataDockWidget.h"

#include "common/Camera.h"

#include <QMessageBox>
#include <QScrollBar>

#include <iostream>
#include <math.h>

#include <opencv2/calib3d/calib3d.hpp>

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
	connect(m_ui->estimatePushButton, &QPushButton::clicked, this, &CalibrationDataDockWidget::estimateCalibrationParameters);
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

void CalibrationDataDockWidget::estimateCalibrationParameters()
{
	common::Camera *camera = m_sensor->camera();

	std::vector<std::vector<cv::Point3f>> objectPoints;
	std::vector<std::vector<cv::Point2f>> imagePoints;

	for (common::Shot *shot : camera->shots())
	{
		QPair<const ccs::common::Pattern*, std::vector<cv::Point2f>> patternData = shot->patternData(m_sensor);
		const common::Pattern *pattern = patternData.first;

		if (pattern == nullptr)
			continue;

		std::vector<cv::Point3f> frameObjectPoints;

		for (int r = 0; r < pattern->cornerCountY(); r++)
		{
			for (int c = 0; c < pattern->cornerCountX(); c++)
				frameObjectPoints.emplace_back(pattern->cornerDistanceX() * c, pattern->cornerDistanceY() * r, 0);
		}

		objectPoints.emplace_back(frameObjectPoints);
		imagePoints.emplace_back(patternData.second);
	}

	if (imagePoints.empty())
	{
		QMessageBox::warning(this, "Estimate parameters", "No images contain any calibration pattern, cannot proceed");
		return;
	}

	cv::Size imageSize(m_sensor->staticInfo().width, m_sensor->staticInfo().height);
	cv::Mat cameraMatrix(3, 3, CV_64FC1);
	std::vector<double> distCoeffs;
	std::vector<cv::Mat> rvec, tvec;

	double error = cv::calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvec, tvec, CV_CALIB_RATIONAL_MODEL);

	m_ui->fxLineEdit->setText(QString::number(cameraMatrix.at<double>(0, 0)));
	m_ui->fyLineEdit->setText(QString::number(cameraMatrix.at<double>(1, 1)));
	m_ui->cxLineEdit->setText(QString::number(cameraMatrix.at<double>(0, 2)));
	m_ui->cyLineEdit->setText(QString::number(cameraMatrix.at<double>(1, 2)));

	m_ui->k1LineEdit->setText(QString::number(distCoeffs[0]));
	m_ui->k2LineEdit->setText(QString::number(distCoeffs[1]));
	m_ui->p1LineEdit->setText(QString::number(distCoeffs[2]));
	m_ui->p2LineEdit->setText(QString::number(distCoeffs[3]));
	m_ui->k3LineEdit->setText(QString::number(distCoeffs[4]));
	m_ui->k4LineEdit->setText(QString::number(distCoeffs[5]));
	m_ui->k5LineEdit->setText(QString::number(distCoeffs[6]));
	m_ui->k6LineEdit->setText(QString::number(distCoeffs[7]));

	//m_ui->k4LineEdit->clear();
	//m_ui->k5LineEdit->clear();
	//m_ui->k6LineEdit->clear();
	m_ui->finalWidthLineEdit->clear();
	m_ui->finalHeightLineEdit->clear();
	m_ui->finalFxLineEdit->clear();
	m_ui->finalFyLineEdit->clear();
	m_ui->finalCxLineEdit->clear();
	m_ui->finalCyLineEdit->clear();

	applyValues();

	QMessageBox::information(this, "Estimate parameters", QString("Re-projection error: %1").arg(error));
}

}
