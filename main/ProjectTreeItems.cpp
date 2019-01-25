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
	connect(m_camera, &common::Camera::liveCaptureRunningChanged, this, &CameraItem::updateIcon);
	updateIcon();

	for (common::Shot *s : camera->shots())
		shotAdded(s);

	connect(m_camera, &common::Camera::shotAdded, this, &CameraItem::shotAdded);
	connect(m_camera, &common::Camera::shotRemoved, this, &CameraItem::shotRemoved);
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

void CameraItem::updateIcon()
{
	if (m_camera->isLiveCaptureRunning())
		setIcon(0, QIcon::fromTheme("media-playback-start"));
	else
		setIcon(0, QIcon::fromTheme("camera-web"));
}

void CameraItem::shotAdded(common::Shot *shot)
{
	ShotItem *item = new ShotItem(m_camera, shot);
	m_shotItems.insert(shot, item);
	addChild(item);
}

void CameraItem::shotRemoved(common::Shot *shot)
{
	auto it = m_shotItems.find(shot);
	delete *it;
	m_shotItems.erase(it);
}

ShotItem::ShotItem(common::Camera *camera, common::Shot *shot)
: BaseContentsItem(shot->name())
, m_camera(camera)
, m_shot(shot)
{
	setIcon(0, QIcon::fromTheme("image"));
}

void ShotItem::addToSelection(ProjectTreeDockWidget::Selection *target) const
{
	target->shots.insert(m_shot);
}

void ShotItem::fillCurrentItem(ProjectTreeDockWidget::CurrentItem *target) const
{
	target->type = ProjectTreeDockWidget::CurrentItem::Shot;
	target->camera = m_camera;
	target->shot = m_shot;
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
