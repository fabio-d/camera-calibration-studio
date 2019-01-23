#include "common/Sensor.h"

#include <math.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QImage>

#include <opencv2/imgproc/imgproc.hpp>

namespace ccs::common
{

static const QVector<QPair<double common::Sensor::CalibrationParameters::*, QString>> fields
{
	{ &common::Sensor::CalibrationParameters::fx, "fx" },
	{ &common::Sensor::CalibrationParameters::fy, "fy" },
	{ &common::Sensor::CalibrationParameters::cx, "cx" },
	{ &common::Sensor::CalibrationParameters::cy, "cy" },
	{ &common::Sensor::CalibrationParameters::p1, "p1" },
	{ &common::Sensor::CalibrationParameters::p2, "p2" },
	{ &common::Sensor::CalibrationParameters::k1, "k1" },
	{ &common::Sensor::CalibrationParameters::k2, "k2" },
	{ &common::Sensor::CalibrationParameters::k3, "k3" },
	{ &common::Sensor::CalibrationParameters::k4, "k4" },
	{ &common::Sensor::CalibrationParameters::k5, "k5" },
	{ &common::Sensor::CalibrationParameters::k6, "k6" }
};

Sensor::Sensor(SqliteDatabase *db, int sensorId)
: m_db(db)
, m_sensorId(sensorId)
{
	QSqlRecord queryCamera = m_db->execReturnOne("SELECT * FROM sensor WHERE id=?", {sensorId});

	m_name = queryCamera.value("name").toString();
	m_width = queryCamera.value("width").toInt();
	m_height = queryCamera.value("height").toInt();

	QJsonObject cp = QJsonDocument::fromJson(queryCamera.value("calibration_parameters").toByteArray()).object();
	for (const auto &it : fields)
	{
		double v = cp.contains(it.second) ? cp.value(it.second).toDouble() : NAN;
		m_calibrationParameters.*it.first = v;
	}
}

Sensor::~Sensor()
{
}

const Sensor::CalibrationParameters &Sensor::calibrationParameters() const
{
	return m_calibrationParameters;
}

void Sensor::setCalibrationParameters(const CalibrationParameters &p)
{
	m_calibrationParameters = p;

	QJsonObject cp;
	for (const auto &it : fields)
	{
		double v = p.*it.first;
		if (!std::isnan(v))
			cp.insert(it.second, v);
	}

	m_db->exec("UPDATE sensor SET calibration_parameters=? WHERE id=?",
		 {QJsonDocument(cp).toJson(), m_sensorId});

	emit calibrationParametersChanged();
}

QImage Sensor::renderImage(const cv::Mat &input, ImageType imageType) const
{
	QImage img(input.cols, input.rows, QImage::Format_RGB888);
	cv::Mat qtImg(img.height(), img.width(), CV_8UC3, img.bits());
	cv::cvtColor(input, qtImg, cv::COLOR_BGR2RGB);

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
