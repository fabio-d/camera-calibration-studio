#pragma once

#include <QDialog>

namespace ccs::main
{

class Ui_AddPatternDialog;

class AddPatternDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit AddPatternDialog(QWidget *parent = nullptr);
		~AddPatternDialog();

		QString patternName() const;
		int cornerCountX() const;
		int cornerCountY() const;

	private:
		void updateControls();

		Ui_AddPatternDialog *m_ui;
};

}
