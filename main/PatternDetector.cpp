#include "main/PatternDetector.h"

#include "common/Camera.h"
#include "common/Pattern.h"

#include <QApplication>
#include <QProgressDialog>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace ccs::main
{

PatternDetector::PatternDetector(const QSet<common::Shot*> &shots, const PatternDetectorConfiguration &config, QWidget *parent)
: m_parent(parent)
, m_shots(shots)
, m_config(config)
{
}

PatternDetector::~PatternDetector()
{
}

void PatternDetector::exec()
{
	QProgressDialog progress(m_parent);
	progress.setLabelText("Detecting pattern...");
	progress.setWindowModality(Qt::WindowModal);
	progress.setMaximum(m_shots.count());

	CornerRefiner cornerRef(m_config.cornerRefinerConfig);
	cv::Size patternSize(m_config.pattern->cornerCountX(), m_config.pattern->cornerCountY());

	int flags = 0;
	if (m_config.normalizeImage)
		flags |= cv::CALIB_CB_NORMALIZE_IMAGE;
	if (m_config.adaptiveThreshold)
		flags |= cv::CALIB_CB_ADAPTIVE_THRESH;
	if (m_config.filterQuads)
		flags |= cv::CALIB_CB_FILTER_QUADS;
	if (m_config.fastCheck)
		flags |= cv::CALIB_CB_FAST_CHECK;

	int ctr = 0;

	QList<std::function<void()>> applyResult;

	for (common::Shot *shot : m_shots)
	{
		progress.setValue(++ctr);

		for (common::Sensor *sensor : shot->camera()->sensors())
		{
			if (!m_config.affectedSensors.contains(sensor))
				continue;

			bool alreadyHasPattern = shot->patternData(sensor).first != nullptr;
			if (m_config.policy == PatternDetectorConfiguration::NeverOverwrite && alreadyHasPattern)
				continue;

			cv::Mat image = shot->sensorData(sensor);
			if (image.empty())
				continue;

			cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);

			std::vector<cv::Point2f> coordinates;
			bool r = cv::findChessboardCorners(image, patternSize, coordinates, flags);

			if (r)
				cornerRef.apply(image, coordinates);

			applyResult << [=]()
			{
				if (r)
					shot->setPatternData(sensor, m_config.pattern, coordinates);
				else if (m_config.policy == PatternDetectorConfiguration::AlwaysOverwrite)
					shot->unsetPatternData(sensor);
			};
		}

		QApplication::processEvents();
		if (progress.wasCanceled())
			return;
	}

	// Apply results
	for (auto x : applyResult)
		x();
}

}
