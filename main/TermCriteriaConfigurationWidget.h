#pragma once

#include <QGroupBox>

#include <opencv2/core/core.hpp>

namespace ccs::main
{

class Ui_TermCriteriaConfigurationWidget;

class TermCriteriaConfigurationWidget : public QGroupBox
{
	Q_OBJECT

	public:
		TermCriteriaConfigurationWidget(QWidget *parent = nullptr);
		~TermCriteriaConfigurationWidget();

		void setValue(const cv::TermCriteria &value);
		cv::TermCriteria value() const;

	private:
		Ui_TermCriteriaConfigurationWidget *m_ui;
};

}
