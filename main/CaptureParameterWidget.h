#pragma once

#include "common/LiveCapture.h"

#include <QWidget>

namespace ccs::main
{

class Ui_CaptureParameterWidget;

class CaptureParameterWidget : public QWidget
{
	Q_OBJECT

	public:
		CaptureParameterWidget(common::BaseLiveCaptureParameter *param, QWidget *parent = nullptr);
		~CaptureParameterWidget();

	private:
		void setup();
		void changed();
		void restore();

		Ui_CaptureParameterWidget *m_ui;

		common::BaseLiveCaptureParameter *m_param;
		bool m_loading;
};

}
