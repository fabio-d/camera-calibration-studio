#pragma once

#include "common/Camera.h"

namespace ccs::main
{

class ImageImporter
{
	public:
		ImageImporter(common::Camera *camera, QWidget *parent = nullptr);
		~ImageImporter();

		void exec();

	private:
		QWidget *m_parent;
		common::Camera *m_camera;
};

}
