#pragma once

#include "common/LiveCapture.h"

#include <QMap>
#include <QSocketNotifier>

namespace ccs::camera_V4L2
{

class LiveCapture : public common::LiveCapture
{
	Q_OBJECT

	public:
		LiveCapture(const QJsonObject &pluginData);
		~LiveCapture();

		QList<common::BaseLiveCaptureParameter*> parameterList() const override;

	private:
		bool checkDeviceMatches(const QJsonObject &pluginData);
		bool configureDevice();
		void mapBuffers();
		void unmapBuffers();
		void streamOn();

		bool dequeueNextEvent();
		bool dequeueNextFrame();
		void frameCallback();

		int m_fd;
		QSocketNotifier *m_fdMonitor;

		cv::Size m_imageSize;
		QList<void*> m_buffers;
		QMap<void*, size_t> m_bufferSize;
		QMap<uint32_t, common::BaseLiveCaptureParameter*> m_parameters;
		int m_bytesPerLine;
};

}
