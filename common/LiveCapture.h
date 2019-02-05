#pragma once

#include <QObject>

#include <opencv2/core/core.hpp>

namespace ccs::common
{

class BaseLiveCaptureParameter : public QObject
{
	Q_OBJECT

	public:
		enum Type
		{
			Integer,	// IntegerLiveCaptureParameter
			SingleChoice	// SingleChoiceLiveCaptureParameter
		};

		virtual ~BaseLiveCaptureParameter();

		const QString &name() const;
		virtual Type type() const = 0;

		// the parameter is read-only (cannot be set) or write-only
		// (cannot be read - the default value will be read instead)
		virtual bool canBeRead() const;
		virtual bool canBeWritten() const;

	signals:
		// Either the current or default/min/max/step values changed
		// because of external events (i.e. not because it was changed
		// through this object)
		void somethingChanged();

	protected:
		explicit BaseLiveCaptureParameter(const QString &name);

	private:
		QString m_name;
};

class IntegerLiveCaptureParameter : public BaseLiveCaptureParameter
{
	public:
		explicit IntegerLiveCaptureParameter(const QString &name);

		Type type() const override;

		virtual void setValue(int value);
		virtual int value() const;

		// this parameter can be rendered as a slider
		virtual bool suggestSlider() const;

		virtual int defaultValue() const;
		virtual int minimumValue() const;
		virtual int maximumValue() const;
		virtual int step() const;
};

// incl. boolean parameters (e.g. enable/disable) too
class SingleChoiceLiveCaptureParameter : public BaseLiveCaptureParameter
{
	public:
		SingleChoiceLiveCaptureParameter(const QString &name, const QStringList &choices);

		Type type() const override;

		const QStringList &choices() const;

		virtual void setCurrentChoice(int index);
		virtual int currentChoice() const;

		virtual int defaultChoice() const;

	private:
		QStringList m_choices;
};

class LiveCapture : public QObject
{
	Q_OBJECT

	public:
		virtual ~LiveCapture(); // stops the live capture

		virtual QList<BaseLiveCaptureParameter*> parameterList() const;

	signals:
		void parameterListChanged();

		// list of images, one for each sensor
		void frameCaptured(const QList<cv::Mat> &images);

	protected:
		LiveCapture();
};

}
