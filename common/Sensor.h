#pragma once

#include "common/CalibrationParameters.h"
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
	friend class Shot;

	public:
		enum ImageType
		{
			Invalid = 0,
			Original,
			Undistorted,
			Mask
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
