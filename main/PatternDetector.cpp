#include "main/PatternDetector.h"

#include "common/Camera.h"
#include "common/Pattern.h"

#include <QDebug>

#include <opencv2/calib3d/calib3d.hpp>

namespace ccs::main
{

PatternDetector::PatternDetector(const QSet<common::Shot*> &shots, QList<common::Pattern*> availablePatterns, QWidget *parent)
: m_shots(shots)
, m_pattern(availablePatterns[0])
{
}

PatternDetector::~PatternDetector()
{
}

void PatternDetector::exec()
{
	cv::Size patternSize(m_pattern->cornerCountX(), m_pattern->cornerCountY());

	for (common::Shot *shot : m_shots)
	{
		for (common::Sensor *sensor : shot->camera()->sensors())
		{
			cv::Mat image = shot->sensorData(sensor);
			if (image.empty())
				continue;

			std::vector<cv::Point2f> coordinates;
			bool r = cv::findChessboardCorners(image, patternSize, coordinates);

			qCritical() << shot->name() << r << coordinates.size();

			if (r)
				shot->setPatternData(sensor, m_pattern, coordinates);
			else
				shot->unsetPatternData(sensor);
		}
	}
}

}
