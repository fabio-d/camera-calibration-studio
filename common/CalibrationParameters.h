#pragma once

#include <QJsonObject>
#include <QValidator>

#include <opencv2/core/core.hpp>

namespace ccs::common
{

class CalibrationParameters;
class Sensor;

class CalibrationParameter
{
	friend class CalibrationParameters;

	public:
		class MetaInfo
		{
			public:
				enum Type
				{
					AnyDouble,
					AnyInt,
					PositiveInt,
				};

				MetaInfo(const QString &name, Type type);

				const QString &name() const;

				// Set/get value
				void setValueFromText(CalibrationParameters *target, const QString &value) const;
				QString valueAsText(const CalibrationParameters *source) const;

				void setValueFromJson(CalibrationParameters *target, const QJsonValue &value) const;
				QJsonValue valueAsJson(const CalibrationParameters *source) const;

				double valueAsDouble(const CalibrationParameters *source, double valueIfNotSet) const;
				int valueAsInt(const CalibrationParameters *source, int valueIfNotSet) const;

				// Create a suitable QValidator
				QValidator *createValidator(QObject *parent = nullptr) const;

			private:
				QString m_name;
				Type m_type;
		};

		CalibrationParameter();
		explicit CalibrationParameter(const MetaInfo *metaInfo);

	private:
		const MetaInfo *m_metaInfo;
		bool m_isSet;
		union
		{
			double asDouble;
			int asInt;
		} m_data;
};

class CalibrationParameters
{
	friend class CalibrationParameter;

	public:
		CalibrationParameters();
		explicit CalibrationParameters(const QJsonObject &deserializeFrom);

		static const QList<CalibrationParameter::MetaInfo> &allFields();

		QJsonObject serialize() const;

		cv::Mat cameraMatrix(const Sensor *sensor) const;
		std::vector<double> distCoeffs(const Sensor *sensor) const;
		cv::Size newSize(const Sensor *sensor) const;
		cv::Mat newCameraMatrix(const Sensor *sensor) const;

	private:
		double readDouble(const QString &name, double valueIfNotSet) const;
		int readInt(const QString &name, int valueIfNotSet) const;

		QMap<QString, CalibrationParameter> m_params;
};

}
