#pragma once

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QVariantList>

namespace ccs::common
{

class SqliteDatabase
{
	public:
		explicit SqliteDatabase(const QString &path);
		~SqliteDatabase();

		bool isOpenOk() const;

		// Run a query and, in case of SELECT, return all the results
		QList<QSqlRecord> exec(const QString &query, const QVariantList &args = {});

		// Run a SELECT query and return the only record
		QSqlRecord execReturnOne(const QString &query, const QVariantList &args = {});

		// Run an INSERT query and return the ID of the last newly-created record
		int execInsertId(const QString &query, const QVariantList &args = {});

	private:
		QSqlQuery bindAndExec(const QString &query, const QVariantList &args);

		QSqlDatabase m_db;
		bool m_openOk;
};

}
