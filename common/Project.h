#pragma once

#include "common/SqliteDatabase.h"

#include <QJsonObject>
#include <QObject>
#include <QSet>

namespace ccs::common
{

class Camera;
struct CameraStaticInfo;
class Pattern;

class Project : public QObject
{
	Q_OBJECT

	public:
		static Project *createOrOpen(const QString &filePath);
		~Project();

		const QSet<Camera*> &cameras() const;
		Camera *addCamera(const QString &cameraName, const CameraStaticInfo &info);
		void removeCamera(Camera *camera);

		const QSet<Pattern*> &patterns() const;
		Pattern *addPattern(const QString &patternName, int cornerCountX, int cornerCountY);
		void removePattern(Pattern *pattern);

	signals:
		void cameraAdded(Camera *camera);
		void cameraRemoved(Camera *camera);
		void patternAdded(Pattern *pattern);
		void patternRemoved(Pattern *pattern);

	private:
		explicit Project(SqliteDatabase *db);

		SqliteDatabase *m_db;
		QSet<Camera*> m_cameras;
		QSet<Pattern*> m_patterns;
};

}
