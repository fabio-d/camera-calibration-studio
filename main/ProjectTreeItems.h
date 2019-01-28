#pragma once

#include "main/ProjectTreeDockWidget.h"

#include <QTreeWidgetItem>

namespace ccs::main
{

class CameraItem;
class PatternItem;
class ShotItem;

class BaseContentsItem : public QObject, public QTreeWidgetItem
{
	Q_OBJECT

	public:
		explicit BaseContentsItem(const QString &text);

		virtual void addToSelection(ProjectTreeDockWidget::Selection *target) const;
		virtual void fillCurrentItem(ProjectTreeDockWidget::CurrentItem *target) const;
};

class CamerasFolderItem : public BaseContentsItem
{
	public:
		explicit CamerasFolderItem(common::Project *project);

		void highlightCamera(common::Camera *camera);

	private:
		void cameraAdded(common::Camera *camera);
		void cameraRemoved(common::Camera *camera);

		QMap<common::Camera*, CameraItem*> m_cameraItems;
};

class CameraItem : public BaseContentsItem
{
	public:
		explicit CameraItem(common::Camera *camera);

		void addToSelection(ProjectTreeDockWidget::Selection *target) const override;
		void fillCurrentItem(ProjectTreeDockWidget::CurrentItem *target) const override;

	private:
		void updateIcon();
		void shotAdded(common::Shot *shot);
		void shotRemoved(common::Shot *shot);

		common::Camera *m_camera;
		QMap<common::Shot*, ShotItem*> m_shotItems;
};

class ShotItem : public BaseContentsItem
{
	public:
		explicit ShotItem(common::Camera *camera, common::Shot *shot);

		void addToSelection(ProjectTreeDockWidget::Selection *target) const override;
		void fillCurrentItem(ProjectTreeDockWidget::CurrentItem *target) const override;

	private:
		common::Camera *m_camera;
		common::Shot *m_shot;
};

class PatternsFolderItem : public BaseContentsItem
{
	public:
		explicit PatternsFolderItem(common::Project *project);

		void highlightPattern(common::Pattern *pattern);

	private:
		void patternAdded(common::Pattern *pattern);
		void patternRemoved(common::Pattern *pattern);

		QMap<common::Pattern*, PatternItem*> m_patternItems;
};

class PatternItem : public BaseContentsItem
{
	public:
		explicit PatternItem(common::Pattern *pattern);

		void addToSelection(ProjectTreeDockWidget::Selection *target) const override;
		void fillCurrentItem(ProjectTreeDockWidget::CurrentItem *target) const override;

	private:
		common::Pattern *m_pattern;
};

class BaseSensorItem : public QObject, public QTreeWidgetItem
{
	Q_OBJECT

	public:
		explicit BaseSensorItem(const QString &text);

		virtual common::Sensor *sensor() const = 0;
		virtual common::Sensor::ImageType imageType() const;
};

class SensorItem : public BaseSensorItem
{
	public:
		explicit SensorItem(common::Sensor *sensor);

		common::Sensor *sensor() const override;

	private:
		common::Sensor *m_sensor;
};

class SensorImageTypeItem : public BaseSensorItem
{
	public:
		SensorImageTypeItem(common::Sensor *sensor, const QString &text, common::Sensor::ImageType imageType);

		common::Sensor *sensor() const override;
		common::Sensor::ImageType imageType() const override;

	private:
		common::Sensor *m_sensor;
		common::Sensor::ImageType m_imageType;
};

}
