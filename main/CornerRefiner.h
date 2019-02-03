#pragma once

#include <opencv2/core/core.hpp>

namespace ccs::main
{

struct CornerRefinerConfiguration
{
	bool enable;

	cv::Size winSize, zeroZone;
	cv::TermCriteria termCriteria;
};

class CornerRefiner
{
	public:
		explicit CornerRefiner(const CornerRefinerConfiguration &config);
		~CornerRefiner();

		void apply(const cv::Mat &grayscaleImage, std::vector<cv::Point2f> &inoutCoordinates);

	private:
		CornerRefinerConfiguration m_config;
};

}
