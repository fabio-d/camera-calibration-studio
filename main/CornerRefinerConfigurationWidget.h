#pragma once

#include "main/CornerRefiner.h"

#include <QGroupBox>

namespace ccs::main
{

class Ui_CornerRefinerConfigurationWidget;

class CornerRefinerConfigurationWidget : public QGroupBox
{
	Q_OBJECT

	public:
		CornerRefinerConfigurationWidget(QWidget *parent);
		~CornerRefinerConfigurationWidget();

		CornerRefinerConfiguration configuration() const;

	private:
		Ui_CornerRefinerConfigurationWidget *m_ui;
};

}
