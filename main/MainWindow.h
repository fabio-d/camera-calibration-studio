#pragma once

#include "common/Project.h"

#include <QMainWindow>

namespace ccs::main
{

class Ui_MainWindow;

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = nullptr);
		~MainWindow();

		// projectFilePath.isNull() -> create new project
		bool openProjectPath(const QString &projectFilePath);

	private:
		void projectLoaded(common::Project *newProject);
		bool queryClose();
		void closeEvent(QCloseEvent *event);
		void projectNew();
		void projectOpen();
		void projectSave();
		void projectTreeSelectionChanged();
		void projectTreeCurrentItemChanged();
		void updateLiveCaptureControls();
		void liveCaptureStart();
		void liveCaptureStop();
		void liveCaptureStopAll();
		void liveCaptureShoot();
		void addCamera();
		void addPattern();
		void deleteItems();
		void importImages();
		void detectPattern();
		void clearPattern();
		void printPattern();
		void about();
		void aboutOpenCV();

		Ui_MainWindow *m_ui;

		common::Project *m_currentProject;

		QList<QMetaObject::Connection> m_updateLiveCaptureControlsConnections;
};

}
