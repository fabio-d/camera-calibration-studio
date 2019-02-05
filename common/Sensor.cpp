#include "common/Sensor.h"

#include <math.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QImage>

#include <opencv2/imgproc/imgproc.hpp>

namespace ccs::common
{



Sensor::Sensor(SqliteDatabase *db, common::Camera *camera, int sensorId)
: m_db(db)
, m_camera(camera)
, m_sensorId(sensorId)
{
	QSqlRecord queryCamera = m_db->execReturnOne("SELECT * FROM sensor WHERE id=?", {sensorId});

	m_name = queryCamera.value("name").toString();
	m_width = queryCamera.value("width").toInt();
	m_height = queryCamera.value("height").toInt();

	QJsonObject cp = QJsonDocument::fromJson(queryCamera.value("calibration_parameters").toByteArray()).object();
	m_calibrationParameters = CalibrationParameters(cp);
}

Sensor::~Sensor()
{
}

Camera *Sensor::camera() const
{
	return m_camera;
}

const CalibrationParameters &Sensor::calibrationParameters() const
{
	return m_calibrationParameters;
}

void Sensor::setCalibrationParameters(const CalibrationParameters &p)
{
	m_calibrationParameters = p;

	m_db->exec("UPDATE sensor SET calibration_parameters=? WHERE id=?",
		 {QJsonDocument(p.serialize()).toJson(), m_sensorId});

	emit calibrationParametersChanged();
}

QImage Sensor::renderImage(const cv::Mat &input, ImageType imageType) const
{
	cv::Mat m = imageType == ImageType::Mask ? cv::Mat(input.rows, input.cols, CV_8UC3, cv::Scalar(255, 255, 255)) : input;

	if (imageType != ImageType::Original)
	{
		cv::Mat cameraMatrix = m_calibrationParameters.cameraMatrix(this);
		std::vector<double> distCoeffs = m_calibrationParameters.distCoeffs(this);
		cv::Size newSize = m_calibrationParameters.newSize(this);
		cv::Mat newCameraMatrix = m_calibrationParameters.newCameraMatrix(this);

		if (!cameraMatrix.empty())
		{
			cv::Mat map1, map2;
			cv::initUndistortRectifyMap(cameraMatrix, distCoeffs,
				cv::Mat(), newCameraMatrix, newSize,
				CV_32FC1, map1, map2);

			cv::remap(m.clone(), m, map1, map2, cv::INTER_LINEAR);
		}
		else
		{
			// cannot undistort, just show a black image
			m = cv::Mat(newSize, CV_8UC3, cv::Scalar());
		}
	}

	QImage img(m.cols, m.rows, QImage::Format_RGB888);
	cv::Mat qtImg(img.height(), img.width(), CV_8UC3, img.bits(), img.bytesPerLine());
	cv::cvtColor(m, qtImg, cv::COLOR_BGR2RGB);

	return img;
}

SensorStaticInfo Sensor::staticInfo() const
{
	SensorStaticInfo r;
	r.name = m_name;
	r.width = m_width;
	r.height = m_height;
	return r;
}

}
