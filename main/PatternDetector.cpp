#include "main/PatternDetector.h"

#include "common/Camera.h"
#include "common/Pattern.h"

#include <QDebug>

#include <opencv2/calib3d/calib3d.hpp>

namespace ccs::main
{

PatternDetector::PatternDetector(const QSet<common::Shot*> &shots, const PatternDetectorConfiguration &config, QWidget *parent)
: m_shots(shots)
, m_config(config)
{
}

PatternDetector::~PatternDetector()
{
}

void PatternDetector::exec()
{
	cv::Size patternSize(m_config.pattern->cornerCountX(), m_config.pattern->cornerCountY());

	int flags = 0;
	if (m_config.normalizeImage)
		flags |= cv::CALIB_CB_NORMALIZE_IMAGE;
	if (m_config.adaptiveThreshold)
		flags |= cv::CALIB_CB_ADAPTIVE_THRESH;

	for (common::Shot *shot : m_shots)
	{
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

			std::vector<cv::Point2f> coordinates;
			bool r = cv::findChessboardCorners(image, patternSize, coordinates, flags);

			if (r)
				shot->setPatternData(sensor, m_config.pattern, coordinates);
			else if (m_config.policy == PatternDetectorConfiguration::AlwaysOverwrite)
				shot->unsetPatternData(sensor);
		}
	}
}

}
