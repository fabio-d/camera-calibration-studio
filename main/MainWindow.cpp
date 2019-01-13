#include "main/MainWindow.h"

#include "main/AddCameraDialog.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include "ui_MainWindow.h"

namespace ccs::main
{

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, m_ui(new Ui_MainWindow)
, m_currentProject(nullptr)
{
	m_ui->setupUi(this);

	m_ui->actionNewProject->setShortcut(QKeySequence::New);
	m_ui->actionOpenProject->setShortcut(QKeySequence::Open);
	m_ui->actionQuit->setShortcut(QKeySequence::Quit);
	m_ui->actionDelete->setShortcut(QKeySequence::Delete);

	connect(m_ui->actionAddCamera, &QAction::triggered, this, &MainWindow::addCamera);
	connect(m_ui->actionDelete, &QAction::triggered, this, &MainWindow::deleteItems);
	connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);

	connect(m_ui->processTreeDockWidget, &ProjectTreeDockWidget::selectionChanged, this, &MainWindow::projectTreeSelectionChanged);
	connect(m_ui->processTreeDockWidget, &ProjectTreeDockWidget::currentItemChanged, this, &MainWindow::projectTreeCurrentItemChanged);
	projectTreeSelectionChanged();
	projectTreeCurrentItemChanged();
}

MainWindow::~MainWindow()
{
	projectLoaded(nullptr);

	delete m_ui;
}

bool MainWindow::openProject(const QString &projectFilePath)
{
	QString fileName;

	if (projectFilePath.isEmpty()) // create new
	{
		fileName = QFileDialog::getSaveFileName(this, "Create project", QString(), "Camera Calibration Studio(*.camcalstu)");
		if (fileName.isEmpty())
			return false;

		// Delete if existing, because it will be reinitialized
		QFile file(fileName);
		if (file.exists())
		{
			if (!QFileInfo(file).isFile() || !file.remove())
				return false;
		}
	}
	else // open existing
	{
		QFileInfo fileInfo(projectFilePath);
		fileName = fileInfo.absoluteFilePath();

		if (!fileInfo.isFile())
			return false;
	}

	common::Project *newProject = common::Project::createOrOpen(fileName);
	if (newProject == nullptr)
	{
		QMessageBox::warning(this, "Open project", "Failed to open project file");
		return false;
	}

	setWindowFilePath(fileName);
	projectLoaded(newProject);

	return true;
}

void MainWindow::projectLoaded(common::Project *newProject)
{
	m_ui->processTreeDockWidget->setProject(nullptr);
	delete m_currentProject;

	m_currentProject = newProject;
	m_ui->processTreeDockWidget->setProject(newProject);
}

void MainWindow::projectTreeSelectionChanged()
{
	m_ui->actionDelete->setEnabled(m_ui->processTreeDockWidget->selectedItems().cameras.isEmpty() == false);
}

void MainWindow::projectTreeCurrentItemChanged()
{
	ProjectTreeDockWidget::CurrentItem it = m_ui->processTreeDockWidget->currentItem();

	m_ui->calibrationDataDockWidget->showSensor(it.sensor);

	qCritical() << it.type << it.camera << it.sensor << it.imageType;
}

void MainWindow::addCamera()
{
	AddCameraDialog d;
	if (d.exec() != QDialog::Accepted)
		return;

	common::Camera *c = m_currentProject->addCamera(d.cameraName(), d.cameraStaticInfo());
	m_ui->processTreeDockWidget->highlightCamera(c);
}

void MainWindow::deleteItems()
{
	ProjectTreeDockWidget::Selection items = m_ui->processTreeDockWidget->selectedItems();

	for (common::Camera *c : items.cameras)
		m_currentProject->removeCamera(c);
}

}
