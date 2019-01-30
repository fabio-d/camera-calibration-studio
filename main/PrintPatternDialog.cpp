#include "main/PrintPatternDialog.h"

#include <QApplication>
#include <QPainter>
#include <QPrinter>

namespace ccs::main
{

PrintPatternDialog::PrintPatternDialog(common::Pattern *pattern, QWidget *parent)
: QPrintPreviewDialog(parent)
, m_pattern(pattern)
{
	connect(this, &QPrintPreviewDialog::paintRequested, this, &PrintPatternDialog::print);

	printer()->setCreator(QApplication::applicationName());
	printer()->setDocName(pattern->name());
	printer()->setColorMode(QPrinter::GrayScale);
	printer()->setPageOrientation(QPageLayout::Landscape);
}

PrintPatternDialog::~PrintPatternDialog()
{
}

void PrintPatternDialog::print(QPrinter *printer)
{
	double dotsPerMm = printer->resolution() / 25.4;
	QPainter p(printer);
	QRectF vp = p.viewport();

	p.translate(vp.center());
	p.scale(dotsPerMm, dotsPerMm);

	double fullSizeX = m_pattern->cornerDistanceX() * (m_pattern->cornerCountX() + 1);
	double fullSizeY = m_pattern->cornerDistanceY() * (m_pattern->cornerCountY() + 1);
	bool invert = false;

	p.translate(-fullSizeX / 2, -fullSizeY / 2);

	for (int c = 0; c <= m_pattern->cornerCountX(); c++)
	{
		for (int r = 0; r <= m_pattern->cornerCountY(); r++)
		{
			if (invert == !((r+c)%2))
				continue;

			QRectF cell(c * m_pattern->cornerDistanceX(), r * m_pattern->cornerDistanceY(),
				m_pattern->cornerDistanceX(), m_pattern->cornerDistanceY());

			p.fillRect(cell, Qt::black);
		}
	}
}

}
