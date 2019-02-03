#include "main/CalibrationAlgorithmConfigurationDialog.h"

#include <QPushButton>

#include <opencv2/calib3d/calib3d.hpp>

#include "ui_CalibrationAlgorithmConfigurationDialog.h"

namespace ccs::main
{

CalibrationAlgorithmConfigurationDialog::CalibrationAlgorithmConfigurationDialog(QWidget *parent)
: QDialog(parent)
, m_ui(new Ui_CalibrationAlgorithmConfigurationDialog)
{
	m_ui->setupUi(this);

	QDoubleValidator *v = new QDoubleValidator(this);
	v->setBottom(0);
	m_ui->xAspectRatioLineEdit->setValidator(v);
	m_ui->yAspectRatioLineEdit->setValidator(v);

	connect(m_ui->enableRationalModelCheckBox, &QCheckBox::toggled, this, &CalibrationAlgorithmConfigurationDialog::updateControls);
	connect(m_ui->enableThinPrismModelCheckBox, &QCheckBox::toggled, this, &CalibrationAlgorithmConfigurationDialog::updateControls);
	connect(m_ui->enableTiltedModelCheckBox, &QCheckBox::toggled, this, &CalibrationAlgorithmConfigurationDialog::updateControls);
	connect(m_ui->noGuessRadioButton, &QCheckBox::toggled, this, &CalibrationAlgorithmConfigurationDialog::updateControls);
	connect(m_ui->withGuessRadioButton, &QCheckBox::toggled, this, &CalibrationAlgorithmConfigurationDialog::updateControls);
	connect(m_ui->xAspectRatioLineEdit, &QLineEdit::textChanged, this, &CalibrationAlgorithmConfigurationDialog::updateControls);
	connect(m_ui->yAspectRatioLineEdit, &QLineEdit::textChanged, this, &CalibrationAlgorithmConfigurationDialog::updateControls);
	updateControls();

	m_ui->termCriteriaConfigWidget->setValue(cv::TermCriteria(
		cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 30, DBL_EPSILON));
}

CalibrationAlgorithmConfigurationDialog::~CalibrationAlgorithmConfigurationDialog()
{
	delete m_ui;
}

int CalibrationAlgorithmConfigurationDialog::calibrateCameraFlags() const
{
	int flags = 0;

	if (!m_ui->enableTangentialDistortionCheckBox->isChecked())
		flags |= cv::CALIB_ZERO_TANGENT_DIST;
	if (m_ui->enableRationalModelCheckBox->isChecked())
		flags |= cv::CALIB_RATIONAL_MODEL;
	if (m_ui->enableThinPrismModelCheckBox->isChecked())
		flags |= cv::CALIB_THIN_PRISM_MODEL;
	if (m_ui->enableTiltedModelCheckBox->isChecked())
		flags |= cv::CALIB_TILTED_MODEL;

	if (m_ui->noGuessRadioButton->isChecked())
	{
		if (m_ui->noGuessFixPrincipalPointCheckBox->isChecked())
			flags |= cv::CALIB_FIX_PRINCIPAL_POINT;
		if (m_ui->noGuessFixAspectRatioCheckBox->isChecked())
			flags |= cv::CALIB_FIX_ASPECT_RATIO;
		if (m_ui->noGuessFixK1CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K1;
		if (m_ui->noGuessFixK2CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K2;
		if (m_ui->noGuessFixK3CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K3;
		if (m_ui->noGuessFixK4CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K4;
		if (m_ui->noGuessFixK5CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K5;
		if (m_ui->noGuessFixK6CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K6;
		if (m_ui->noGuessFixS1234CheckBox->isChecked())
			flags |= cv::CALIB_FIX_S1_S2_S3_S4;
		if (m_ui->noGuessFixTauXYCheckBox->isChecked())
			flags |= cv::CALIB_FIX_TAUX_TAUY;
	}
	else
	{
		assert(m_ui->withGuessRadioButton->isChecked());
		flags |= cv::CALIB_USE_INTRINSIC_GUESS;

		if (m_ui->withGuessFixPrincipalPointCheckBox->isChecked())
			flags |= cv::CALIB_FIX_PRINCIPAL_POINT;
		if (m_ui->withGuessFixAspectRatioCheckBox->isChecked())
			flags |= cv::CALIB_FIX_ASPECT_RATIO;
		if (m_ui->withGuessFixK1CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K1;
		if (m_ui->withGuessFixK2CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K2;
		if (m_ui->withGuessFixK3CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K3;
		if (m_ui->withGuessFixK4CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K4;
		if (m_ui->withGuessFixK5CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K5;
		if (m_ui->withGuessFixK6CheckBox->isChecked())
			flags |= cv::CALIB_FIX_K6;
		if (m_ui->withGuessFixS1234CheckBox->isChecked())
			flags |= cv::CALIB_FIX_S1_S2_S3_S4;
		if (m_ui->withGuessFixTauXYCheckBox->isChecked())
			flags |= cv::CALIB_FIX_TAUX_TAUY;
	}

	return flags;
}

double CalibrationAlgorithmConfigurationDialog::fixedAspectRatio() const
{
	bool xOk, yOk;
	double x = m_ui->xAspectRatioLineEdit->text().toDouble(&xOk);
	double y = m_ui->yAspectRatioLineEdit->text().toDouble(&yOk);

	return (xOk ? x : 1.0) / (yOk ? y : 1.0);
}

cv::TermCriteria CalibrationAlgorithmConfigurationDialog::termCriteria() const
{
	return m_ui->termCriteriaConfigWidget->value();
}

void CalibrationAlgorithmConfigurationDialog::updateControls()
{
	bool rationalModel = m_ui->enableRationalModelCheckBox->isChecked();
	bool thinPrismModel = m_ui->enableThinPrismModelCheckBox->isChecked();
	bool tiltedModel = m_ui->enableTiltedModelCheckBox->isChecked();
	bool withGuess = m_ui->withGuessRadioButton->isChecked();

	m_ui->noGuessFixPrincipalPointCheckBox->setEnabled(!withGuess);
	m_ui->noGuessFixAspectRatioCheckBox->setEnabled(!withGuess);
	m_ui->noGuessFixK1CheckBox->setEnabled(!withGuess);
	m_ui->noGuessFixK2CheckBox->setEnabled(!withGuess);
	m_ui->noGuessFixK3CheckBox->setEnabled(!withGuess);

	if (!withGuess && rationalModel)
	{
		m_ui->noGuessFixK4CheckBox->setEnabled(true);
		m_ui->noGuessFixK5CheckBox->setEnabled(true);
		m_ui->noGuessFixK6CheckBox->setEnabled(true);
	}
	else
	{
		m_ui->noGuessFixK4CheckBox->setEnabled(false);
		m_ui->noGuessFixK5CheckBox->setEnabled(false);
		m_ui->noGuessFixK6CheckBox->setEnabled(false);
		m_ui->noGuessFixK4CheckBox->setChecked(false);
		m_ui->noGuessFixK5CheckBox->setChecked(false);
		m_ui->noGuessFixK6CheckBox->setChecked(false);
	}

	if (!withGuess && thinPrismModel)
	{
		m_ui->noGuessFixS1234CheckBox->setEnabled(true);
	}
	else
	{
		m_ui->noGuessFixS1234CheckBox->setEnabled(false);
		m_ui->noGuessFixS1234CheckBox->setChecked(false);
	}

	if (!withGuess && tiltedModel)
	{
		m_ui->noGuessFixTauXYCheckBox->setEnabled(true);
	}
	else
	{
		m_ui->noGuessFixTauXYCheckBox->setEnabled(false);
		m_ui->noGuessFixTauXYCheckBox->setChecked(false);
	}

	m_ui->withGuessFixPrincipalPointCheckBox->setEnabled(withGuess);
	m_ui->withGuessFixAspectRatioCheckBox->setEnabled(withGuess);
	m_ui->withGuessFixK1CheckBox->setEnabled(withGuess);
	m_ui->withGuessFixK2CheckBox->setEnabled(withGuess);
	m_ui->withGuessFixK3CheckBox->setEnabled(withGuess);

	if (withGuess && rationalModel)
	{
		m_ui->withGuessFixK4CheckBox->setEnabled(true);
		m_ui->withGuessFixK5CheckBox->setEnabled(true);
		m_ui->withGuessFixK6CheckBox->setEnabled(true);
	}
	else
	{
		m_ui->withGuessFixK4CheckBox->setEnabled(false);
		m_ui->withGuessFixK5CheckBox->setEnabled(false);
		m_ui->withGuessFixK6CheckBox->setEnabled(false);
		m_ui->withGuessFixK4CheckBox->setChecked(false);
		m_ui->withGuessFixK5CheckBox->setChecked(false);
		m_ui->withGuessFixK6CheckBox->setChecked(false);
	}

	if (withGuess && thinPrismModel)
	{
		m_ui->withGuessFixS1234CheckBox->setEnabled(true);
	}
	else
	{
		m_ui->withGuessFixS1234CheckBox->setEnabled(false);
		m_ui->withGuessFixS1234CheckBox->setChecked(false);
	}

	if (withGuess && tiltedModel)
	{
		m_ui->withGuessFixTauXYCheckBox->setEnabled(true);
	}
	else
	{
		m_ui->withGuessFixTauXYCheckBox->setEnabled(false);
		m_ui->withGuessFixTauXYCheckBox->setChecked(false);
	}

	bool xOk = m_ui->xAspectRatioLineEdit->hasAcceptableInput();
	bool yOk = m_ui->yAspectRatioLineEdit->hasAcceptableInput();

	// default "1" is acceptable too
	if (!xOk && m_ui->xAspectRatioLineEdit->text().isEmpty())
		xOk = true;
	if (!yOk && m_ui->yAspectRatioLineEdit->text().isEmpty())
		yOk = true;

	m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(xOk && yOk);
}

}
