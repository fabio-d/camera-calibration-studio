#pragma once

#include "common/LiveCapture.h"

#include <QMap>

namespace ccs::camera_V4L2
{

common::BaseLiveCaptureParameter *createParameterObject(int fd, uint32_t controlId);

class IntegerLiveCaptureParameter : public common::IntegerLiveCaptureParameter
{
	public:
		IntegerLiveCaptureParameter(int fd, uint32_t controlId);

		// common::BaseLiveCaptureParameter
		bool canBeRead() const override;
		bool canBeWritten() const override;

		// common::IntegerLiveCaptureParameter
		void setValue(int value) override;
		int value() const override;
		bool suggestSlider() const override;
		int defaultValue() const override;
		int minimumValue() const override;
		int maximumValue() const override;
		int step() const override;

	private:
		int m_fd;
		uint32_t m_controlId;
};

class MenuLiveCaptureParameter : public common::SingleChoiceLiveCaptureParameter
{
	public:
		MenuLiveCaptureParameter(int fd, uint32_t controlId, const QMap<int, QString> &enumMenu);

		// common::BaseLiveCaptureParameter
		bool canBeRead() const override;
		bool canBeWritten() const override;

		// common::SingleChoiceLiveCaptureParameter
		void setCurrentChoice(int index) override;
		int currentChoice() const override;
		int defaultChoice() const override;

	private:
		int m_fd;
		uint32_t m_controlId;
		QList<int> m_indexMap;
};

}
