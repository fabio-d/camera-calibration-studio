#include "main/AddPatternDialog.h"

#include <QPushButton>

#include "ui_AddPatternDialog.h"

namespace ccs::main
{

AddPatternDialog::AddPatternDialog(QWidget *parent)
: QDialog(parent)
, m_ui(new Ui_AddPatternDialog)
{
	m_ui->setupUi(this);

	connect(m_ui->cornerCountXSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddPatternDialog::updateControls);
	connect(m_ui->cornerCountYSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &AddPatternDialog::updateControls);

	updateControls();
}

AddPatternDialog::~AddPatternDialog()
{
	delete m_ui;
}

void AddPatternDialog::updateControls()
{
	m_ui->patternNameLineEdit->setPlaceholderText(
		QString("%1x%2 pattern")
			.arg(cornerCountX()).arg(cornerCountY())
		);

	m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

QString AddPatternDialog::patternName() const
{
	QString r = m_ui->patternNameLineEdit->text();
	if (r.isEmpty())
		r = m_ui->patternNameLineEdit->placeholderText();
	return r;
}

int AddPatternDialog::cornerCountX() const
{
	return m_ui->cornerCountXSpinBox->value();
}

int AddPatternDialog::cornerCountY() const
{
	return m_ui->cornerCountYSpinBox->value();
}

}
