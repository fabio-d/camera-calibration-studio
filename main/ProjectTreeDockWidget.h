#pragma once

#include "common/Camera.h"
#include "common/Pattern.h"
#include "common/Project.h"
#include "common/Sensor.h"

#include <QDockWidget>

namespace ccs::main
{

class CamerasFolderItem;
class PatternsFolderItem;
class SensorItem;

class Ui_ProjectTreeDockWidget;

class ProjectTreeDockWidget : public QDockWidget
{
	Q_OBJECT

	public:
		struct Selection
		{
			QSet<common::Camera*> cameras;
			QSet<common::Shot*> shots;
			QSet<common::Pattern*> patterns;
		};

		struct CurrentItem
		{
			enum
			{
				None,
				Camera,
				Shot,
				Pattern
			} type;

			common::Camera *camera; // Camera
			common::Shot *shot; // Camera, Shot

			common::Sensor *sensor; // Camera, Shot
			common::Sensor::ImageType imageType; // Camera, Shot

			common::Pattern *pattern; // Pattern
		};

		ProjectTreeDockWidget(QWidget *parent);
		~ProjectTreeDockWidget();

		void setProject(common::Project *project);

		void highlightCamera(common::Camera *camera);
		void highlightPattern(common::Pattern *pattern);

		Selection selectedItems() const;
		CurrentItem currentItem() const;

	signals:
		void selectionChanged();
		void currentItemChanged();

	private:
		void updateSensorTree(common::Camera *camera);
		void contentsCurrentItemChanged();
		void sensorCurrentItemChanged();

		Ui_ProjectTreeDockWidget *m_ui;

		bool m_updatingSensorTree;
		CamerasFolderItem *m_camerasFolder;
		PatternsFolderItem *m_patternsFolder;
		QMultiMap<QString, SensorItem*> m_displayedSensors;
};

}
