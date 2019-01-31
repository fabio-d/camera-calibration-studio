#pragma once

#include "common/Shot.h"

#include <QSet>
#include <QWidget>

namespace ccs::main
{

class PatternDetector
{
	public:
		explicit PatternDetector(const QSet<common::Shot*> &shots, QList<common::Pattern*> availablePatterns, QWidget *parent = nullptr);
		~PatternDetector();

		void exec();

	private:
		QSet<common::Shot*> m_shots;
		common::Pattern *m_pattern;
};

}
