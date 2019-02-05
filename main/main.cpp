#include "main/MainWindow.h"

#include "common/CameraPlugin.h"

#include <QApplication>
#include <QMessageBox>

using namespace ccs;

int main(int argc, char *argv[])
{
	QApplication::setApplicationName("Camera Calibration Studio");
	QApplication::setApplicationVersion("0.1");
	QApplication::setOrganizationName("Department of Mathematics and Computer Science (DMI), University of Catania");
	QApplication::setOrganizationDomain("dmi.unict.it");

	QApplication app(argc, argv);
	QStringList args = QApplication::arguments();

	common::CameraPlugin::detectAvailablePlugins(app.applicationDirPath());

	main::MainWindow mw;
	mw.show();

	if (!mw.openProjectPath(args.count() == 2 ? args[1] : QString::null))
	{
		QMessageBox::critical(&mw, "Error", "Failed to open " + args[1]);
		return EXIT_FAILURE;
	}

	return app.exec();
}
