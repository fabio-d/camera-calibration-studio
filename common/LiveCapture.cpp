#include "common/LiveCapture.h"

namespace ccs::common
{

BaseLiveCaptureParameter::BaseLiveCaptureParameter(const QString &name)
: m_name(name)
{
}

BaseLiveCaptureParameter::~BaseLiveCaptureParameter()
{
}

const QString &BaseLiveCaptureParameter::name() const
{
	return m_name;
}

bool BaseLiveCaptureParameter::canBeRead() const
{
	return true;
}

bool BaseLiveCaptureParameter::canBeWritten() const
{
	return true;
}

IntegerLiveCaptureParameter::IntegerLiveCaptureParameter(const QString &name)
: BaseLiveCaptureParameter(name)
{
}

BaseLiveCaptureParameter::Type IntegerLiveCaptureParameter::type() const
{
	return BaseLiveCaptureParameter::Integer;
}

void IntegerLiveCaptureParameter::setValue(int value)
{
}

int IntegerLiveCaptureParameter::value() const
{
	return 0;
}

bool IntegerLiveCaptureParameter::suggestSlider() const
{
	return true;
}

int IntegerLiveCaptureParameter::defaultValue() const
{
	return 0;
}

int IntegerLiveCaptureParameter::minimumValue() const
{
	return 0;
}

int IntegerLiveCaptureParameter::maximumValue() const
{
	return 0;
}

int IntegerLiveCaptureParameter::step() const
{
	return 1;
}

SingleChoiceLiveCaptureParameter::SingleChoiceLiveCaptureParameter(const QString &name, const QStringList &choices)
: BaseLiveCaptureParameter(name)
, m_choices(choices)
{
}

BaseLiveCaptureParameter::Type SingleChoiceLiveCaptureParameter::type() const
{
	return BaseLiveCaptureParameter::SingleChoice;
}

const QStringList &SingleChoiceLiveCaptureParameter::choices() const
{
	return m_choices;
}

void SingleChoiceLiveCaptureParameter::setCurrentChoice(int index)
{
}

int SingleChoiceLiveCaptureParameter::currentChoice() const
{
	return 0;
}

int SingleChoiceLiveCaptureParameter::defaultChoice() const
{
	return 0;
}

LiveCapture::LiveCapture()
{
}

LiveCapture::~LiveCapture()
{
}

QList<BaseLiveCaptureParameter*> LiveCapture::parameterList() const
{
	return QList<BaseLiveCaptureParameter*>();
}

}
