#pragma once

#include "common/SqliteDatabase.h"

#include <QObject>

#include <opencv2/core/core.hpp>

namespace ccs::common
{

struct SensorStaticInfo
{
	QString name;
	int width, height;
};

class Sensor : public QObject
{
	Q_OBJECT

	friend class Camera;

	public:
		enum ImageType
		{
			Invalid = 0,
			Original,
			Undistorted,
			Mask
		};

		struct CalibrationParameters
		{
			// each of these value can be NAN if not set by the user
			double fx, fy, cx, cy;
			double p1, p2;
			double k1, k2, k3, k4, k5, k6;
		};

		~Sensor();

		const CalibrationParameters &calibrationParameters() const;
		void setCalibrationParameters(const CalibrationParameters &p);

		QImage renderImage(const cv::Mat &input, ImageType imageType) const;

		SensorStaticInfo staticInfo() const;

	signals:
		void calibrationParametersChanged();

	private:
		Sensor(SqliteDatabase *db, int sensorId);

		SqliteDatabase *m_db;
		int m_sensorId;

		CalibrationParameters m_calibrationParameters;

		// SensorStaticInfo
		QString m_name;
		int m_width, m_height;
};

}
