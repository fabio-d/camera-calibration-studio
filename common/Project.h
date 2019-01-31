#pragma once

#include "common/SqliteDatabase.h"

#include <QJsonObject>
#include <QObject>

namespace ccs::common
{

class Camera;
struct CameraStaticInfo;
class Pattern;

class Project : public QObject
{
	Q_OBJECT

	friend class Shot;

	public:
		static Project *createOrOpen(const QString &filePath);
		~Project();

		bool isDirtyState() const;
		bool commit();

		QList<Camera*> cameras() const;
		Camera *addCamera(const QString &cameraName, const CameraStaticInfo &info);
		void removeCamera(Camera *camera);

		QList<Pattern*> patterns() const;
		Pattern *addPattern(const QString &patternName, int cornerCountX, int cornerCountY);
		void removePattern(Pattern *pattern);

	signals:
		void dirtyStateChanged(bool dirtyState);
		void cameraAdded(Camera *camera);
		void cameraRemoved(Camera *camera);
		void patternAdded(Pattern *pattern);
		void patternRemoved(Pattern *pattern);

	private:
		explicit Project(SqliteDatabase *db);

		void dbDirtyStateChanged();

		SqliteDatabase *m_db;
		QMap<int, Camera*> m_cameras;
		QMap<int, Pattern*> m_patterns;
};

}
