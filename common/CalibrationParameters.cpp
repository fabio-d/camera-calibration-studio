#include "common/CalibrationParameters.h"
#include "common/Sensor.h"

#include <assert.h>
#include <math.h>

#include <QDoubleValidator>
#include <QIntValidator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QImage>

namespace ccs::common
{

static QList<CalibrationParameter::MetaInfo> allFieldsMetaInfo
{
	{ "fx", CalibrationParameter::MetaInfo::AnyDouble },
	{ "fy", CalibrationParameter::MetaInfo::AnyDouble },
	{ "cx", CalibrationParameter::MetaInfo::AnyDouble },
	{ "cy", CalibrationParameter::MetaInfo::AnyDouble },

	{ "p1", CalibrationParameter::MetaInfo::AnyDouble },
	{ "p2", CalibrationParameter::MetaInfo::AnyDouble },
	{ "k1", CalibrationParameter::MetaInfo::AnyDouble },
	{ "k2", CalibrationParameter::MetaInfo::AnyDouble },
	{ "k3", CalibrationParameter::MetaInfo::AnyDouble },
	{ "k4", CalibrationParameter::MetaInfo::AnyDouble },
	{ "k5", CalibrationParameter::MetaInfo::AnyDouble },
	{ "k6", CalibrationParameter::MetaInfo::AnyDouble },

	{ "finalHeight", CalibrationParameter::MetaInfo::PositiveInt },
	{ "finalWidth", CalibrationParameter::MetaInfo::PositiveInt },
	{ "finalFx", CalibrationParameter::MetaInfo::AnyDouble },
	{ "finalFy", CalibrationParameter::MetaInfo::AnyDouble },
	{ "finalCx", CalibrationParameter::MetaInfo::AnyDouble },
	{ "finalCy", CalibrationParameter::MetaInfo::AnyDouble }
};

CalibrationParameter::MetaInfo::MetaInfo(const QString &name, Type type)
: m_name(name)
, m_type(type)
{
}

const QString &CalibrationParameter::MetaInfo::name() const
{
	return m_name;
}

void CalibrationParameter::MetaInfo::setValueFromText(CalibrationParameters *target, const QString &value) const
{
	CalibrationParameter &p = target->m_params[m_name];
	assert(p.m_metaInfo == this);

	if (value.isEmpty())
	{
		p.m_isSet = false;
	}
	else if (m_type == AnyDouble)
	{
		p.m_data.asDouble = value.toDouble();
		p.m_isSet = true;
	}
	else
	{
		assert(m_type == AnyInt || m_type == PositiveInt);
		p.m_data.asInt = value.toInt();
		p.m_isSet = true;
	}
}

QString CalibrationParameter::MetaInfo::valueAsText(const CalibrationParameters *source) const
{
	const CalibrationParameter &p = source->m_params[m_name];
	assert(p.m_metaInfo == this);

	if (p.m_isSet == false)
	{
		return QString::null;
	}
	else if (m_type == AnyDouble)
	{
		return QString::number(p.m_data.asDouble);
	}
	else
	{
		assert(m_type == AnyInt || m_type == PositiveInt);
		return QString::number(p.m_data.asInt);
	}
}

void CalibrationParameter::MetaInfo::setValueFromJson(CalibrationParameters *target, const QJsonValue &value) const
{
	CalibrationParameter &p = target->m_params[m_name];
	assert(p.m_metaInfo == this);

	if (value.isNull())
	{
		p.m_isSet = false;
	}
	else if (m_type == AnyDouble)
	{
		p.m_data.asDouble = value.toDouble();
		p.m_isSet = true;
	}
	else
	{
		assert(m_type == AnyInt || m_type == PositiveInt);
		p.m_data.asInt = value.toInt();
		p.m_isSet = true;
	}
}

QJsonValue CalibrationParameter::MetaInfo::valueAsJson(const CalibrationParameters *source) const
{
	const CalibrationParameter &p = source->m_params[m_name];
	assert(p.m_metaInfo == this);

	if (p.m_isSet == false)
	{
		return QJsonValue(); // null
	}
	else if (m_type == AnyDouble)
	{
		return p.m_data.asDouble;
	}
	else
	{
		assert(m_type == AnyInt || m_type == PositiveInt);
		return p.m_data.asInt;
	}
}

double CalibrationParameter::MetaInfo::valueAsDouble(const CalibrationParameters *source, double valueIfNotSet) const
{
	const CalibrationParameter &p = source->m_params[m_name];
	assert(p.m_metaInfo == this);
	assert(m_type == AnyDouble);

	return p.m_isSet ? p.m_data.asDouble : valueIfNotSet;
}

int CalibrationParameter::MetaInfo::valueAsInt(const CalibrationParameters *source, int valueIfNotSet) const
{
	const CalibrationParameter &p = source->m_params[m_name];
	assert(p.m_metaInfo == this);
	assert(m_type == AnyInt || m_type == PositiveInt);

	return p.m_isSet ? p.m_data.asInt : valueIfNotSet;
}

QValidator *CalibrationParameter::MetaInfo::createValidator(QObject *parent) const
{
	if (m_type == AnyDouble)
	{
		return new QDoubleValidator(parent);
	}
	else
	{
		assert(m_type == AnyInt || m_type == PositiveInt);

		QIntValidator *v = new QIntValidator(parent);
		if (m_type == PositiveInt)
			v->setBottom(1);
		return v;
	}
}

CalibrationParameter::CalibrationParameter()
: m_metaInfo(nullptr)
{
}

CalibrationParameter::CalibrationParameter(const MetaInfo *metaInfo)
: m_metaInfo(metaInfo),
  m_isSet(false)
{
}

CalibrationParameters::CalibrationParameters()
{
	for (const CalibrationParameter::MetaInfo &m : allFields())
		m_params.insert(m.name(), CalibrationParameter(&m));
		
}

CalibrationParameters::CalibrationParameters(const QJsonObject &deserializeFrom)
: CalibrationParameters()
{
	for (const CalibrationParameter::MetaInfo &m : allFields())
		m.setValueFromJson(this, deserializeFrom.value(m.name()));
}

const QList<CalibrationParameter::MetaInfo> &CalibrationParameters::allFields()
{
	return allFieldsMetaInfo;
}

QJsonObject CalibrationParameters::serialize() const
{
	QJsonObject r;

	for (const CalibrationParameter::MetaInfo &m : allFields())
		r.insert(m.name(), m.valueAsJson(this));

	return r;
}

cv::Mat CalibrationParameters::cameraMatrix(const Sensor *sensor) const
{
	double fx = readDouble("fx", NAN);
	if (std::isnan(fx))
		return cv::Mat();

	return cv::Mat_<double>(3,3)<<
		fx,
		0,
		readDouble("cx", sensor->staticInfo().width * 0.5),

		0,
		readDouble("fy", fx),
		readDouble("cy", sensor->staticInfo().height * 0.5),

		0,
		0,
		1;
}

std::vector<double> CalibrationParameters::distCoeffs(const Sensor *sensor) const
{
	std::vector<double> distCoeffs;
	distCoeffs.push_back(readDouble("k1", 0));
	distCoeffs.push_back(readDouble("k2", 0));
	distCoeffs.push_back(readDouble("p1", 0));
	distCoeffs.push_back(readDouble("p2", 0));
	distCoeffs.push_back(readDouble("k3", 0));
	distCoeffs.push_back(readDouble("k4", 0));
	distCoeffs.push_back(readDouble("k5", 0));
	distCoeffs.push_back(readDouble("k6", 0));
	return distCoeffs;
}

cv::Size CalibrationParameters::newSize(const Sensor *sensor) const
{
	return cv::Size(
		readInt("finalWidth", sensor->staticInfo().width),
		readInt("finalHeight", sensor->staticInfo().height)
	);
}

cv::Mat CalibrationParameters::newCameraMatrix(const Sensor *sensor) const
{
	cv::Mat orig = cameraMatrix(sensor);
	if (orig.empty())
		return cv::Mat();

	cv::Size size = newSize(sensor);
	return cv::Mat_<double>(3,3)<<
		readDouble("finalFx", orig.at<double>(0, 0)),
		0,
		readDouble("finalCx", size.width * 0.5),

		0,
		readDouble("finalFy", orig.at<double>(1, 1)),
		readDouble("finalCy", size.height * 0.5),

		0,
		0,
		1;
}

double CalibrationParameters::readDouble(const QString &name, double valueIfNotSet) const
{
	const CalibrationParameter &p = m_params[name];
	return p.m_metaInfo->valueAsDouble(this, valueIfNotSet);
}

int CalibrationParameters::readInt(const QString &name, int valueIfNotSet) const
{
	const CalibrationParameter &p = m_params[name];
	return p.m_metaInfo->valueAsInt(this, valueIfNotSet);
}

}
