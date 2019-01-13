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
		bool openProject(const QString &projectFilePath);

	private:
		void projectLoaded(common::Project *newProject);
		void projectTreeSelectionChanged();
		void projectTreeCurrentItemChanged();
		void addCamera();
		void deleteItems();

		Ui_MainWindow *m_ui;

		common::Project *m_currentProject;
};

}
