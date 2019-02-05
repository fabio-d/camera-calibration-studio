#pragma once

#include "main/CaptureParameterWidget.h"

#include "common/Camera.h"

#include <QDockWidget>

namespace ccs::main
{

class Ui_CaptureParametersDockWidget;

class CaptureParametersDockWidget : public QDockWidget
{
	Q_OBJECT

	public:
		CaptureParametersDockWidget(QWidget *parent);
		~CaptureParametersDockWidget();

		void showNothing();
		void showLiveCapture(common::Camera *camera);

	private:
		void loadLiveCaptureParameterList();

		Ui_CaptureParametersDockWidget *m_ui;

		common::Camera *m_liveCaptureCamera;
		QMetaObject::Connection m_liveCaptureParametersListChanged;

		QList<CaptureParameterWidget*> m_paramWidgets;
};

}
