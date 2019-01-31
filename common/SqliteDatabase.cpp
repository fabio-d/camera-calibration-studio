#include "common/SqliteDatabase.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

namespace ccs::common
{

static void initSchema(SqliteDatabase *db)
{
	db->exec(R"(
		CREATE TABLE IF NOT EXISTS camera(
			id INTEGER PRIMARY KEY,
			name TEXT NOT NULL,
			plugin TEXT,
			plugin_data BLOB
		)
	)");

	db->exec(R"(
		CREATE TABLE IF NOT EXISTS sensor(
			id INTEGER PRIMARY KEY,
			camera_id TEXT NOT NULL,
			name TEXT NOT NULL,
			width INTEGER NOT NULL,
			height INTEGER NOT NULL,
			calibration_parameters BLOB,
			UNIQUE(id, camera_id),
			FOREIGN KEY(camera_id) REFERENCES camera(id) ON DELETE CASCADE
		)
	)");

	db->exec(R"(
		CREATE TABLE IF NOT EXISTS shot(
			id INTEGER PRIMARY KEY,
			camera_id TEXT NOT NULL,
			name TEXT NOT NULL,
			capture_parameters BLOB,
			UNIQUE(id, camera_id),
			FOREIGN KEY(camera_id) REFERENCES camera(id) ON DELETE CASCADE
		)
	)");

	db->exec(R"(
		CREATE TABLE IF NOT EXISTS sensor_data(
			id INTEGER PRIMARY KEY,
			camera_id INTEGER NOT NULL,
			shot_id INTEGER NOT NULL,
			sensor_id INTEGER NOT NULL,
			data BLOB,
			FOREIGN KEY(shot_id, camera_id) REFERENCES shot(id, camera_id) ON DELETE CASCADE,
			FOREIGN KEY(sensor_id, camera_id) REFERENCES sensor(id, camera_id) ON DELETE CASCADE
		)
	)");

	db->exec(R"(
		CREATE TABLE IF NOT EXISTS pattern(
			id INTEGER PRIMARY KEY,
			name TEXT NOT NULL,
			corner_count_x INTEGER NOT NULL,
			corner_count_y INTEGER NOT NULL,
			corner_distance_x DOUBLE,
			corner_distance_y DOUBLE
		)
	)");

	if (!db->commit())
		qFatal("Failed to perform initial commit");
}

SqliteDatabase::SqliteDatabase(const QString &path, QObject *parent)
: m_db(QSqlDatabase::addDatabase("QSQLITE", "ccs_sqlite" + QUuid::createUuid().toString()))
, m_dirtyState(false)
{
	m_db.setDatabaseName(path);

	m_openOk = m_db.open();
	if (m_openOk)
	{
		m_db.exec("PRAGMA foreign_keys = ON");

		if (!m_db.transaction())
			qFatal("Failed to start initial transaction");

		// Ensure that the schema exists
		initSchema(this);
	}
	else
	{
		qCritical() << path << m_db.lastError();
	}
}

SqliteDatabase::~SqliteDatabase()
{
	if (m_openOk)
		m_db.close();

	QString connName = m_db.connectionName();

	m_db = QSqlDatabase(); // drop reference to the DB connection
	QSqlDatabase::removeDatabase(m_db.connectionName());
}

bool SqliteDatabase::isOpenOk() const
{
	return m_openOk;
}

bool SqliteDatabase::isDirtyState() const
{
	return m_dirtyState;
}

bool SqliteDatabase::commit()
{
	if (m_dirtyState)
	{
		if (!m_db.commit())
			return false;

		if (!m_db.transaction())
			qFatal("Failed to start new transaction after commit");

		m_dirtyState = false;
		emit dirtyStateChanged();
	}

	return true;
}

QList<QSqlRecord> SqliteDatabase::exec(const QString &query, const QVariantList &args)
{
	QSqlQuery q = bindAndExec(query, args);
	QList<QSqlRecord> r;

	if (q.isSelect())
	{
		while (q.next())
			r.append(q.record());
	}

	return r;
}

QSqlRecord SqliteDatabase::execReturnOne(const QString &query, const QVariantList &args)
{
	QSqlQuery q = bindAndExec(query, args);

	bool ok = q.next();
	Q_ASSERT(ok == true);

	QSqlRecord result = q.record();

	bool moreRecords = q.next();
	Q_ASSERT(moreRecords == false);

	return result;
}

int SqliteDatabase::execInsertId(const QString &query, const QVariantList &args)
{
	QSqlQuery q = bindAndExec(query, args);
	return q.lastInsertId().toInt();
}

QSqlQuery SqliteDatabase::bindAndExec(const QString &query, const QVariantList &args)
{
	QSqlQuery r(m_db);

	r.prepare(query);
	for (const QVariant &v : args)
		r.addBindValue(v);

	bool ok = r.exec();
	Q_ASSERT(ok == true);

	if (!r.isSelect() && !m_dirtyState)
	{
		m_dirtyState = true;
		emit dirtyStateChanged();
	}

	return r;
}

}
