#include "main/ProjectTreeItems.h"

// TODO: delete objects

namespace ccs::main
{

BaseContentsItem::BaseContentsItem(const QString &text)
: QTreeWidgetItem(QStringList() << text)
{
}

void BaseContentsItem::addToSelection(ProjectTreeDockWidget::Selection *target) const
{
}

void BaseContentsItem::fillCurrentItem(ProjectTreeDockWidget::CurrentItem *target) const
{
}

CamerasFolderItem::CamerasFolderItem(common::Project *project)
: BaseContentsItem("Cameras")
{
	setIcon(0, QIcon::fromTheme("folder"));
	setFlags(flags() & ~Qt::ItemIsSelectable);

	for (common::Camera *c : project->cameras())
		cameraAdded(c);

	connect(project, &common::Project::cameraAdded, this, &CamerasFolderItem::cameraAdded);
	connect(project, &common::Project::cameraRemoved, this, &CamerasFolderItem::cameraRemoved);
}

void CamerasFolderItem::highlightCamera(common::Camera *camera)
{
	CameraItem *it = m_cameraItems.value(camera);
	QTreeWidget *treeWidget = it->treeWidget();

	setExpanded(true);

	treeWidget->clearSelection();
	treeWidget->scrollToItem(it);
	treeWidget->setCurrentItem(it);
	it->setSelected(true);
}

void CamerasFolderItem::cameraAdded(common::Camera *camera)
{
	CameraItem *it = new CameraItem(camera);
	m_cameraItems.insert(camera, it);
	addChild(it);
}

void CamerasFolderItem::cameraRemoved(common::Camera *camera)
{
	CameraItem *it = m_cameraItems.take(camera);
	delete it;
}

CameraItem::CameraItem(common::Camera *camera)
: BaseContentsItem(camera->name())
, m_camera(camera)
{
	setIcon(0, QIcon::fromTheme("camera-web"));

	addChild(new ImageItem(camera, "IMG0001"));
	addChild(new ImageItem(camera, "IMG0002"));
	addChild(new ImageItem(camera, "IMG0003"));
	addChild(new ImageItem(camera, "IMG0004"));
	addChild(new ImageItem(camera, "IMG0005"));
}

void CameraItem::addToSelection(ProjectTreeDockWidget::Selection *target) const
{
	target->cameras.insert(m_camera);
}

void CameraItem::fillCurrentItem(ProjectTreeDockWidget::CurrentItem *target) const
{
	target->type = ProjectTreeDockWidget::CurrentItem::Camera;
	target->camera = m_camera;
}

ImageItem::ImageItem(common::Camera *camera, const QString &text)
: BaseContentsItem(text)
, m_camera(camera)
{
	setIcon(0, QIcon::fromTheme("image"));
}

void ImageItem::addToSelection(ProjectTreeDockWidget::Selection *target) const
{
	//target->cameras.insert(m_camera);
}

void ImageItem::fillCurrentItem(ProjectTreeDockWidget::CurrentItem *target) const
{
	target->type = ProjectTreeDockWidget::CurrentItem::Camera;
	target->camera = m_camera;
}

PatternsFolderItem::PatternsFolderItem(common::Project *project)
: BaseContentsItem("Patterns")
{
	setIcon(0, QIcon::fromTheme("folder"));
	setFlags(flags() & ~Qt::ItemIsSelectable);
}

BaseSensorItem::BaseSensorItem(const QString &text)
: QTreeWidgetItem(QStringList() << text)
{
}

common::Sensor::ImageType BaseSensorItem::imageType() const
{
	return common::Sensor::Invalid;
}

SensorItem::SensorItem(common::Sensor *sensor)
: BaseSensorItem(sensor->staticInfo().name)
, m_sensor(sensor)
{
	setIcon(0, QIcon::fromTheme("preferences-desktop-display-color"));
	setFlags(flags() & ~Qt::ItemIsSelectable);

	addChild(new SensorImageTypeItem(m_sensor, "Original", common::Sensor::Original));
	addChild(new SensorImageTypeItem(m_sensor, "Undistorted", common::Sensor::Undistorted));
	addChild(new SensorImageTypeItem(m_sensor, "Mask", common::Sensor::Mask));
}

common::Sensor *SensorItem::sensor() const
{
	return m_sensor;
}

SensorImageTypeItem::SensorImageTypeItem(common::Sensor *sensor, const QString &text, common::Sensor::ImageType imageType)
: BaseSensorItem(text)
, m_sensor(sensor)
, m_imageType(imageType)
{
	setIcon(0, QIcon::fromTheme("image"));
}

common::Sensor *SensorImageTypeItem::sensor() const
{
	return m_sensor;
}

common::Sensor::ImageType SensorImageTypeItem::imageType() const
{
	return m_imageType;
}

}
