#pragma once

#include <QDialog>

#include <opencv2/core/core.hpp>

namespace ccs::main
{

class Ui_CalibrationAlgorithmConfigurationDialog;

class CalibrationAlgorithmConfigurationDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit CalibrationAlgorithmConfigurationDialog(QWidget *parent = nullptr);
		~CalibrationAlgorithmConfigurationDialog();

		int calibrateCameraFlags() const;
		double fixedAspectRatio() const;
		cv::TermCriteria termCriteria() const;

	private:
		void updateControls();

		Ui_CalibrationAlgorithmConfigurationDialog *m_ui;
};

}
