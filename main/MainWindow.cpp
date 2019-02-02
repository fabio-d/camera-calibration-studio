#include "main/MainWindow.h"

#include "main/AddCameraDialog.h"
#include "main/AddPatternDialog.h"
#include "main/PatternDetector.h"
#include "main/PatternDetectorConfigurationDialog.h"
#include "main/PrintPatternDialog.h"

#include <QCloseEvent>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QUuid>

#include "ui_MainWindow.h"

static const char *ccsFileFilter = "Camera Calibration Studio(*.camcalstu)";

namespace ccs::main
{

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, m_ui(new Ui_MainWindow)
, m_currentProject(nullptr)
{
	m_ui->setupUi(this);

	m_ui->actionNew->setShortcut(QKeySequence::New);
	m_ui->actionOpen->setShortcut(QKeySequence::Open);
	m_ui->actionSave->setShortcut(QKeySequence::Save);
	m_ui->actionQuit->setShortcut(QKeySequence::Quit);
	m_ui->actionDelete->setShortcut(QKeySequence::Delete);

	connect(m_ui->actionNew, &QAction::triggered, this, &MainWindow::projectNew);
	connect(m_ui->actionOpen, &QAction::triggered, this, &MainWindow::projectOpen);
	connect(m_ui->actionSave, &QAction::triggered, this, &MainWindow::projectSave);
	connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(m_ui->actionAddCamera, &QAction::triggered, this, &MainWindow::addCamera);
	connect(m_ui->actionAddPattern, &QAction::triggered, this, &MainWindow::addPattern);
	connect(m_ui->actionDelete, &QAction::triggered, this, &MainWindow::deleteItems);
	connect(m_ui->actionLiveCaptureStart, &QAction::triggered, this, &MainWindow::liveCaptureStart);
	connect(m_ui->actionLiveCaptureStop, &QAction::triggered, this, &MainWindow::liveCaptureStop);
	connect(m_ui->actionLiveCaptureStopAll, &QAction::triggered, this, &MainWindow::liveCaptureStopAll);
	connect(m_ui->actionLiveCaptureShoot, &QAction::triggered, this, &MainWindow::liveCaptureShoot);
	connect(m_ui->actionDetectPattern, &QAction::triggered, this, &MainWindow::detectPattern);
	connect(m_ui->actionPrintPattern, &QAction::triggered, this, &MainWindow::printPattern);
	connect(m_ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
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

bool MainWindow::openProjectPath(const QString &projectFilePath)
{
	QString fileName;

	if (projectFilePath.isEmpty()) // create new
	{
		fileName = QFileDialog::getSaveFileName(this, "Create project", QString(), ccsFileFilter);
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

	if (newProject != nullptr)
	{
		connect(newProject, &common::Project::dirtyStateChanged, this, &QMainWindow::setWindowModified);
		setWindowModified(newProject->isDirtyState());
	}
	else
	{
		setWindowModified(false);
	}
}

bool MainWindow::queryClose()
{
	if (m_currentProject->isDirtyState() == false)
		return true;

	QMessageBox::StandardButton r = QMessageBox::question(this, "Close project",
		"The project has been modified. Do you want to save your changes or discard them?",
		QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);

	switch (r)
	{
		case QMessageBox::Save:
			return m_currentProject->commit();
		case QMessageBox::Discard:
			return true;
		default: // QMessageBox::Cancel
			return false;
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (queryClose())
		event->accept();
	else
		event->ignore();
}

void MainWindow::projectNew()
{
	if (!queryClose())
		return;

	openProjectPath(QString::null);
}

void MainWindow::projectOpen()
{
	if (!queryClose())
		return;

	QString fileName = QFileDialog::getOpenFileName(this, "Open project", QString(), ccsFileFilter);
	if (fileName.isEmpty())
		return;

	openProjectPath(fileName);
}

void MainWindow::projectSave()
{
	m_currentProject->commit();
}

void MainWindow::projectTreeSelectionChanged()
{
	// Disconnect old connections
	for (const QMetaObject::Connection &c : m_updateLiveCaptureControlsConnections)
		disconnect(c);
	m_updateLiveCaptureControlsConnections.clear();

	m_ui->actionDelete->setEnabled(!m_ui->processTreeDockWidget->selectedItems().cameras.isEmpty()
		|| !m_ui->processTreeDockWidget->selectedItems().shots.isEmpty()
		|| !m_ui->processTreeDockWidget->selectedItems().patterns.isEmpty());

	// Create new connections
	for (common::Camera *c : m_ui->processTreeDockWidget->selectedItems().cameras)
	{
		m_updateLiveCaptureControlsConnections <<
			connect(c, &common::Camera::liveCaptureRunningChanged, this, &MainWindow::updateLiveCaptureControls);
	}

	updateLiveCaptureControls();

	m_ui->actionDetectPattern->setEnabled(!m_ui->processTreeDockWidget->selectedItems().cameras.isEmpty()
		|| !m_ui->processTreeDockWidget->selectedItems().shots.isEmpty());
}

void MainWindow::projectTreeCurrentItemChanged()
{
	ProjectTreeDockWidget::CurrentItem it = m_ui->processTreeDockWidget->currentItem();

	if (it.pattern != nullptr)
	{
		assert(it.sensor == nullptr);

		m_ui->calibrationDataDockWidget->showPattern(it.pattern);
		m_ui->centralWidget->showPattern(it.pattern);
	}
	else if (it.sensor != nullptr)
	{
		assert(it.pattern == nullptr);

		m_ui->calibrationDataDockWidget->showSensor(it.sensor);

		if (it.imageType == common::Sensor::Invalid)
			m_ui->centralWidget->showNothing();
		else if (it.type == ProjectTreeDockWidget::CurrentItem::Shot)
			m_ui->centralWidget->showShot(it.shot, it.sensor, it.imageType);
		else
			m_ui->centralWidget->showLiveCapture(it.camera, it.sensor, it.imageType);
	}
	else
	{
		m_ui->calibrationDataDockWidget->showNothing();
		m_ui->centralWidget->showNothing();
	}

	m_ui->actionPrintPattern->setEnabled(it.pattern != nullptr);
}

void MainWindow::updateLiveCaptureControls()
{
	// Check selected cameras
	bool haveSelectedCamerasThatCanStartLiveCapture = false;
	bool haveSelectedCamerasThatCanStopLiveCapture = false;
	for (common::Camera *c : m_ui->processTreeDockWidget->selectedItems().cameras)
	{
		if (c->isLiveCaptureRunning())
			haveSelectedCamerasThatCanStopLiveCapture = true;
		else if (c->supportsLiveCapture())
			haveSelectedCamerasThatCanStartLiveCapture = true;
	}

	// Check all cameras
	bool haveCamerasWithRunningLiveCapture = false;
	if (m_currentProject != nullptr)
	{
		for (common::Camera *c : m_currentProject->cameras())
		{
			if (c->isLiveCaptureRunning())
				haveCamerasWithRunningLiveCapture = true;
		}
	}

	m_ui->actionLiveCaptureStart->setEnabled(haveSelectedCamerasThatCanStartLiveCapture);
	m_ui->actionLiveCaptureStop->setEnabled(haveSelectedCamerasThatCanStopLiveCapture);
	m_ui->actionLiveCaptureStopAll->setEnabled(haveCamerasWithRunningLiveCapture);
	m_ui->actionLiveCaptureShoot->setEnabled(haveCamerasWithRunningLiveCapture);
}

void MainWindow::liveCaptureStart()
{
	for (common::Camera *c : m_ui->processTreeDockWidget->selectedItems().cameras)
	{
		if (c->supportsLiveCapture())
			c->startLiveCapture();
	}
}

void MainWindow::liveCaptureStop()
{
	for (common::Camera *c : m_ui->processTreeDockWidget->selectedItems().cameras)
		c->stopLiveCapture();
}

void MainWindow::liveCaptureStopAll()
{
	for (common::Camera *c : m_currentProject->cameras())
		c->stopLiveCapture();

	updateLiveCaptureControls();
}

void MainWindow::liveCaptureShoot()
{
	QString randomName = "Image" + QUuid::createUuid().toString();

	if (m_currentProject != nullptr)
	{
		for (common::Camera *c : m_currentProject->cameras())
		{
			if (c->isLiveCaptureRunning())
			{
				QMap<const common::Sensor*, cv::Mat> frame = c->lastCapturedFrame();
				if (!frame.empty())
					c->addShot(randomName, {}, frame);
			}
		}
	}
}

void MainWindow::addCamera()
{
	AddCameraDialog d;
	if (d.exec() != QDialog::Accepted)
		return;

	common::Camera *c = m_currentProject->addCamera(d.cameraName(), d.cameraStaticInfo());
	m_ui->processTreeDockWidget->highlightCamera(c);
}

void MainWindow::addPattern()
{
	AddPatternDialog d;
	if (d.exec() != QDialog::Accepted)
		return;

	common::Pattern *p = m_currentProject->addPattern(d.patternName(), d.cornerCountX(), d.cornerCountY());
	p->setCornerDistanceX(25.0);
	p->setCornerDistanceY(p->cornerDistanceX());

	m_ui->processTreeDockWidget->highlightPattern(p);
}

void MainWindow::deleteItems()
{
	ProjectTreeDockWidget::Selection items = m_ui->processTreeDockWidget->selectedItems();

	for (common::Camera *c : items.cameras)
		m_currentProject->removeCamera(c);

	for (common::Shot *s : items.shots)
		s->camera()->removeShot(s);

	for (common::Pattern *p : items.patterns)
		m_currentProject->removePattern(p);

	// the pattern may have been removed
	m_ui->centralWidget->updateImage();
}

static QList<common::Sensor*> listReferencedSensorsInAlphabeticalOrder(const QSet<common::Shot*> &shots)
{
	struct MyPair
	{
		common::Camera* camera;
		common::Sensor* sensor;

		bool operator<(const MyPair &other) const
		{
			int cComp = camera->name().compare(other.camera->name(), Qt::CaseInsensitive);
			if (cComp != 0)
				return cComp < 0;

			return sensor->staticInfo().name.compare(other.sensor->staticInfo().name, Qt::CaseInsensitive) < 0;
		}
	};

	QMap<MyPair, common::Sensor*> cont;
	for (common::Shot *s : shots)
	{
		common::Camera *c = s->camera();
		for (common::Sensor *s : c->sensors())
			cont.insert({c, s}, s);
	}

	return cont.values();
}

void MainWindow::detectPattern()
{
	ProjectTreeDockWidget::Selection items = m_ui->processTreeDockWidget->selectedItems();

	QSet<common::Shot*> shots = items.shots;
	for (common::Camera *c : items.cameras)
		shots |= c->shots();

	QList<common::Sensor*> sensors = listReferencedSensorsInAlphabeticalOrder(shots);
	if (sensors.isEmpty())
		return;

	PatternDetectorConfigurationDialog d(m_currentProject->patterns(), sensors, this);
	if (d.exec() != QDialog::Accepted)
		return;

	PatternDetector w(shots, d.configuration(), this);
	w.exec();

	// pattern data may have changed
	m_ui->centralWidget->updateImage();
}

void MainWindow::printPattern()
{
	common::Pattern *p = m_ui->processTreeDockWidget->currentItem().pattern;
	assert(p != nullptr);

	PrintPatternDialog d(p, this);
	d.exec();
}

void MainWindow::about()
{
	// NOTE: Use same style as "About Qt"

	QMessageBox::about(this,
		QApplication::applicationName(),
		"<h3>About " + QApplication::applicationName() + "</h3>"
		"<p>" + QApplication::applicationName() + " " + QApplication::applicationVersion() + "</p>"
		"<p><a href=\"" + QApplication::organizationDomain() + "\">" + QApplication::organizationName() + "</a></p>"
	);
}

}
