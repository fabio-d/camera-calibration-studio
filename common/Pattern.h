#pragma once

#include "common/SqliteDatabase.h"

#include <QObject>

namespace ccs::common
{

class Pattern : public QObject
{
	Q_OBJECT

	friend class Project;
	friend class Shot;

	public:
		~Pattern();

		const QString &name() const;

		int cornerCountX() const;
		int cornerCountY() const;

		void setCornerDistanceX(double v);
		double cornerDistanceX() const;

		void setCornerDistanceY(double v);
		double cornerDistanceY() const;

	signals:
		void geometryChanged();

	private:
		Pattern(SqliteDatabase *db, int patternId);

		SqliteDatabase *m_db;
		int m_patternId;

		QString m_name;
		int m_cornerCountX, m_cornerCountY;
		double m_cornerDistanceX, m_cornerDistanceY;
};

}
