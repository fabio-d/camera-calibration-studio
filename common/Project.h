#pragma once

#include "common/SqliteDatabase.h"

#include <QJsonObject>
#include <QObject>
#include <QSet>

namespace ccs::common
{

class Camera;
struct CameraStaticInfo;

class Project : public QObject
{
	Q_OBJECT

	public:
		static Project *createOrOpen(const QString &filePath);
		~Project();

		const QSet<Camera*> &cameras() const;
		Camera *addCamera(const QString &cameraName, const CameraStaticInfo &info);
		void removeCamera(Camera *camera);

	signals:
		void cameraAdded(Camera *camera);
		void cameraRemoved(Camera *camera);

	private:
		explicit Project(SqliteDatabase *db);

		SqliteDatabase *m_db;
		QSet<Camera*> m_cameras;
};

}
