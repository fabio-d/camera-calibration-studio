#include "main/ImageImporter.h"

#include <QFileDialog>
#include <QMessageBox>

#include <opencv2/highgui/highgui.hpp>

namespace ccs::main
{

ImageImporter::ImageImporter(common::Camera *camera, QWidget *parent)
: m_parent(parent)
, m_camera(camera)
{
}

ImageImporter::~ImageImporter()
{
}

// HACK TODO : redo it properly
void ImageImporter::exec()
{
	common::Sensor *sensor = m_camera->sensors()[0];
	cv::Size expectedSize(sensor->staticInfo().width, sensor->staticInfo().height);

	QStringList files = QFileDialog::getOpenFileNames(m_parent,
		"Select one or more image files to import",
		QString(), "Images(*.png *.jpg)");

	QStringList errors;

	for (const QString &path : files)
	{
		cv::Mat image = cv::imread(path.toUtf8().constData());
		if (image.size() != expectedSize)
		{
			if (image.empty())
				errors << QString("%1: Failed to load image").arg(path);
			else
				errors << QString("%5: Invalid image size (must be %1x%2, got %3x%4)")
					.arg(expectedSize.width).arg(expectedSize.height)
					.arg(image.cols).arg(image.rows).arg(path);

			continue;
		}

		// Ensure we have a unique name among the shots in the selected camera
		QString shotName = QFileInfo(path).completeBaseName();
		int ctr = 0;
		while (m_camera->project()->shotNameAlreadyExists(shotName, m_camera))
			shotName = QFileInfo(path).completeBaseName() + QString("-%1").arg(++ctr);

		QMap<const common::Sensor*, cv::Mat> frame;
		frame.insert(sensor, image);

		m_camera->addShot(shotName, {}, frame);
	}

	if (!errors.isEmpty())
	{
		QMessageBox d(QMessageBox::Warning, "Import error", "One or more files could not be imported", QMessageBox::Ok);
		d.setDetailedText(errors.join("\n"));
		d.exec();
	}
}

}
