#pragma once

#include "common/LiveCapture.h"

#include <QSocketNotifier>

namespace ccs::camera_V4L2
{

class LiveCapture : public common::LiveCapture
{
	Q_OBJECT

	public:
		LiveCapture(const QJsonObject &pluginData);
		~LiveCapture();

	private:
		bool checkDeviceMatches(const QJsonObject &pluginData);
		bool configureDevice();
		void mapBuffers();
		void unmapBuffers();
		void streamOn();

		cv::Mat readNextFrame();
		void frameCallback();

		int m_fd;
		QSocketNotifier *m_fdMonitor;

		cv::Size m_imageSize;
		QList<void*> m_buffers;
};

}