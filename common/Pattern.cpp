#include "common/Pattern.h"

namespace ccs::common
{

Pattern::Pattern(SqliteDatabase *db, int patternId)
: m_db(db)
, m_patternId(patternId)
{
	QSqlRecord queryPattern = m_db->execReturnOne("SELECT * FROM pattern WHERE id=?", {patternId});

	m_name = queryPattern.value("name").toString();
	m_cornerCountX = queryPattern.value("corner_count_x").toInt();
	m_cornerCountY = queryPattern.value("corner_count_y").toInt();
	m_cornerDistanceX = queryPattern.value("corner_distance_x").toInt();
	m_cornerDistanceY = queryPattern.value("corner_distance_y").toInt();
}

Pattern::~Pattern()
{
}

const QString &Pattern::name() const
{
	return m_name;
}

int Pattern::cornerCountX() const
{
	return m_cornerCountX;
}

int Pattern::cornerCountY() const
{
	return m_cornerCountY;
}

void Pattern::setCornerDistanceX(double v)
{
	m_cornerDistanceX = v;

	m_db->exec("UPDATE pattern SET corner_distance_x=? WHERE id=?",
		 {m_cornerDistanceX, m_patternId});

	emit geometryChanged();
}

double Pattern::cornerDistanceX() const
{
	return m_cornerDistanceX;
}

void Pattern::setCornerDistanceY(double v)
{
	m_cornerDistanceY = v;

	m_db->exec("UPDATE pattern SET corner_distance_y=? WHERE id=?",
		 {m_cornerDistanceY, m_patternId});

	emit geometryChanged();
}

double Pattern::cornerDistanceY() const
{
	return m_cornerDistanceY;
}

}
