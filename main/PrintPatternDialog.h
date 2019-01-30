#pragma once

#include "common/Pattern.h"

#include <QPrintPreviewDialog>

namespace ccs::main
{

class PrintPatternDialog : public QPrintPreviewDialog
{
	Q_OBJECT

	public:
		explicit PrintPatternDialog(common::Pattern *pattern, QWidget *parent = nullptr);
		~PrintPatternDialog();

	private:
		void print(QPrinter *printer);

		common::Pattern *m_pattern;
};

}
