#pragma once

#include <QObject>

#include <opencv2/core/core.hpp>

namespace ccs::common
{

class LiveCapture : public QObject
{
	Q_OBJECT

	public:
		virtual ~LiveCapture(); // stops the live capture

	signals:
		// list of images, one for each sensor
		void frameCaptured(const QList<cv::Mat> &images);

	protected:
		LiveCapture();
};

}
