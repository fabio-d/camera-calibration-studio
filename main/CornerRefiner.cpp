#include "main/CornerRefiner.h"

#include <opencv2/imgproc/imgproc.hpp>

namespace ccs::main
{

CornerRefiner::CornerRefiner(const CornerRefinerConfiguration &config)
: m_config(config)
{
}

CornerRefiner::~CornerRefiner()
{
}

void CornerRefiner::apply(const cv::Mat &grayscaleImage, std::vector<cv::Point2f> &inoutCoordinates)
{
	if (!m_config.enable)
		return;

	cv::cornerSubPix(grayscaleImage, inoutCoordinates,
		m_config.winSize, m_config.zeroZone, m_config.termCriteria);
}

}
