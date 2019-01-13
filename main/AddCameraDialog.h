#pragma once

#include "common/AddCameraWidget.h"
#include "common/Camera.h"

#include <QDialog>
#include <QMap>

namespace ccs::main
{

class Ui_AddCameraDialog;

class AddCameraDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit AddCameraDialog(QWidget *parent = nullptr);
		~AddCameraDialog();

		QString cameraName() const;
		common::CameraStaticInfo cameraStaticInfo() const;

	private:
		void selectWidget();
		void updateControls();

		Ui_AddCameraDialog *m_ui;
		QMap<QString, common::AddCameraWidget*> m_widgets;
};

}
