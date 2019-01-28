#include "main/ProjectTreeDockWidget.h"

#include "main/ProjectTreeItems.h"

#include "ui_ProjectTreeDockWidget.h"

namespace ccs::main
{

ProjectTreeDockWidget::ProjectTreeDockWidget(QWidget *parent)
: QDockWidget(parent)
, m_ui(new Ui_ProjectTreeDockWidget)
, m_updatingSensorTree(false)
{
	m_ui->setupUi(this);

	connect(m_ui->contentsTreeWidget, &QTreeWidget::itemSelectionChanged, this, &ProjectTreeDockWidget::selectionChanged);
	connect(m_ui->contentsTreeWidget, &QTreeWidget::currentItemChanged, this, &ProjectTreeDockWidget::contentsCurrentItemChanged);
	connect(m_ui->sensorTreeWidget, &QTreeWidget::currentItemChanged, this, &ProjectTreeDockWidget::sensorCurrentItemChanged);
	contentsCurrentItemChanged();
	sensorCurrentItemChanged();

	setProject(nullptr);
}

ProjectTreeDockWidget::~ProjectTreeDockWidget()
{
	delete m_ui;
}

void ProjectTreeDockWidget::setProject(common::Project *project)
{
	m_camerasFolder = nullptr;
	m_patternsFolder = nullptr;
	m_ui->contentsTreeWidget->clear();

	if (project == nullptr)
		return;

	m_camerasFolder = new CamerasFolderItem(project);
	m_ui->contentsTreeWidget->addTopLevelItem(m_camerasFolder);

	m_patternsFolder = new PatternsFolderItem(project);
	m_ui->contentsTreeWidget->addTopLevelItem(m_patternsFolder);

	m_camerasFolder->setExpanded(true);
	m_patternsFolder->setExpanded(true);
}

void ProjectTreeDockWidget::highlightCamera(common::Camera *camera)
{
	m_camerasFolder->highlightCamera(camera);
}

void ProjectTreeDockWidget::highlightPattern(common::Pattern *pattern)
{
	m_patternsFolder->highlightPattern(pattern);
}

ProjectTreeDockWidget::Selection ProjectTreeDockWidget::selectedItems() const
{
	Selection r;

	for (QTreeWidgetItem *it : m_ui->contentsTreeWidget->selectedItems())
		static_cast<BaseContentsItem*>(it)->addToSelection(&r);

	for (common::Camera *c : r.cameras)
		r.shots -= c->shots();

	return r;
}

ProjectTreeDockWidget::CurrentItem ProjectTreeDockWidget::currentItem() const
{
	CurrentItem r;
	memset(&r, 0, sizeof(CurrentItem));
	r.type = CurrentItem::None;

	QTreeWidgetItem *cIt = m_ui->contentsTreeWidget->currentItem();
	if (cIt != nullptr)
		static_cast<BaseContentsItem*>(cIt)->fillCurrentItem(&r);

	QTreeWidgetItem *sIt = m_ui->sensorTreeWidget->currentItem();
	if (sIt != nullptr)
	{
		BaseSensorItem *s = static_cast<BaseSensorItem*>(sIt);
		r.sensor = s->sensor();
		r.imageType = s->imageType();
	}

	return r;
}

void ProjectTreeDockWidget::updateSensorTree(common::Camera *camera)
{
	m_updatingSensorTree = true;
	m_ui->sensorTreeWidget->clear();

	if (camera == nullptr)
	{
		m_ui->sensorTreeWidget->setEnabled(false);
	}
	else
	{
		m_ui->sensorTreeWidget->setEnabled(true);

		for (common::Sensor *s : camera->sensors())
		{
			QTreeWidgetItem *sIt = new SensorItem(s);
			m_ui->sensorTreeWidget->addTopLevelItem(sIt);
			sIt->setExpanded(true);
		}
	}

	m_updatingSensorTree = false;
}

void ProjectTreeDockWidget::contentsCurrentItemChanged()
{
	ProjectTreeDockWidget::CurrentItem it = currentItem();

	switch (it.type)
	{
		case ProjectTreeDockWidget::CurrentItem::Camera:
		case ProjectTreeDockWidget::CurrentItem::Shot:
			updateSensorTree(it.camera);
			break;
		default:
			updateSensorTree(nullptr);
			break;
	}

	emit currentItemChanged();
}

void ProjectTreeDockWidget::sensorCurrentItemChanged()
{
	if (m_updatingSensorTree) // inhibit notifications while updating
		return;

	emit currentItemChanged();
}

}
