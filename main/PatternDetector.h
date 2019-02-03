#pragma once

#include "main/CornerRefiner.h"

#include "common/Shot.h"

#include <QSet>
#include <QWidget>

namespace ccs::main
{

struct PatternDetectorConfiguration
{
	common::Pattern *pattern;
	bool normalizeImage, adaptiveThreshold, filterQuads, fastCheck;
	CornerRefinerConfiguration cornerRefinerConfig;
	QSet<common::Sensor*> affectedSensors;
	enum OverwritePolicy
	{
		NeverOverwrite,
		OverwriteIfFound,
		AlwaysOverwrite
	} policy;
};

class PatternDetector
{
	public:
		explicit PatternDetector(const QSet<common::Shot*> &shots, const PatternDetectorConfiguration &config, QWidget *parent = nullptr);
		~PatternDetector();

		void exec();

	private:
		QWidget *m_parent;
		QSet<common::Shot*> m_shots;
		PatternDetectorConfiguration m_config;
};

}
