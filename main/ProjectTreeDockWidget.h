#pragma once

#include "common/Camera.h"
#include "common/Project.h"
#include "common/Sensor.h"

#include <QDockWidget>

namespace ccs::main
{

class CamerasFolderItem;
class PatternsFolderItem;

class Ui_ProjectTreeDockWidget;

class ProjectTreeDockWidget : public QDockWidget
{
	Q_OBJECT

	public:
		struct Selection
		{
			QSet<common::Camera*> cameras;
		};

		struct CurrentItem
		{
			enum
			{
				None,
				Camera
			} type;

			common::Camera *camera;

			common::Sensor *sensor;
			common::Sensor::ImageType imageType;
		};

		ProjectTreeDockWidget(QWidget *parent);
		~ProjectTreeDockWidget();

		void setProject(common::Project *project);

		void highlightCamera(common::Camera *camera);

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
};

}
