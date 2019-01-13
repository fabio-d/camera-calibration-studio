#include "main/AddCameraDialog.h"

#include "common/CameraPlugin.h"

#include <QPushButton>

#include "ui_AddCameraDialog.h"

namespace ccs::main
{

AddCameraDialog::AddCameraDialog(QWidget *parent)
: QDialog(parent)
, m_ui(new Ui_AddCameraDialog)
{
	m_ui->setupUi(this);
	setTabOrder(m_ui->pluginComboBox, m_ui->pluginStackedWidget);

	connect(m_ui->pluginComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AddCameraDialog::selectWidget);

	// Fill plugin selection combobox
	for (const common::CameraPluginMetadata &m : common::CameraPlugin::availablePluginsMetadata())
		m_ui->pluginComboBox->addItem(m.pluginName, m.pluginId);

	updateControls();
}

AddCameraDialog::~AddCameraDialog()
{
	delete m_ui;
}

void AddCameraDialog::selectWidget()
{
	Q_ASSERT(m_ui->pluginComboBox->currentData().isValid());

	QString pluginId = m_ui->pluginComboBox->currentData().toString();

	auto it = m_widgets.find(pluginId);
	if (it == m_widgets.end())
	{
		it = m_widgets.insert(
			pluginId,
			common::CameraPlugin::pluginInstance(pluginId)->createAddCameraWidget(this)
		);

		m_ui->pluginStackedWidget->addWidget(*it);
		connect(*it, &common::AddCameraWidget::defaultCameraNameChanged, this, &AddCameraDialog::updateControls);
		connect(*it, &common::AddCameraWidget::canAcceptChanged, this, &AddCameraDialog::updateControls);
	}

	m_ui->pluginStackedWidget->setCurrentWidget(*it);
}

void AddCameraDialog::updateControls()
{
	common::AddCameraWidget *w = static_cast<common::AddCameraWidget*>(
		m_ui->pluginStackedWidget->currentWidget());
	bool canAccept = w && w->canAccept();

	if (w)
		m_ui->cameraNameLineEdit->setPlaceholderText(w->defaultCameraName());
	else
		m_ui->cameraNameLineEdit->setPlaceholderText(QString::null);

	m_ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(canAccept);
}

QString AddCameraDialog::cameraName() const
{
	QString r = m_ui->cameraNameLineEdit->text();
	if (r.isEmpty())
		r = m_ui->cameraNameLineEdit->placeholderText();
	return r;
}

common::CameraStaticInfo AddCameraDialog::cameraStaticInfo() const
{
	common::AddCameraWidget *w = static_cast<common::AddCameraWidget*>(
		m_ui->pluginStackedWidget->currentWidget());

	common::CameraStaticInfo r;
	r.pluginId = m_ui->pluginComboBox->currentData().toString();
	r.pluginData = w->pluginData();
	r.sensors = w->sensorsStaticInfo();

	return r;
}

}
