#pragma once

#include <QSqlDatabase>
#include <QSqlRecord>
#include <QVariantList>

namespace ccs::common
{

class SqliteDatabase : public QObject
{
	Q_OBJECT

	public:
		explicit SqliteDatabase(const QString &path, QObject *parent = nullptr);
		~SqliteDatabase();

		bool isOpenOk() const;

		bool isDirtyState() const;
		bool commit();

		// Run a query and, in case of SELECT, return all the results
		QList<QSqlRecord> exec(const QString &query, const QVariantList &args = {});

		// Run a SELECT query and return the only record
		QSqlRecord execReturnOne(const QString &query, const QVariantList &args = {});

		// Run an INSERT query and return the ID of the last newly-created record
		int execInsertId(const QString &query, const QVariantList &args = {});

	signals:
		void dirtyStateChanged();

	private:
		QSqlQuery bindAndExec(const QString &query, const QVariantList &args);

		QSqlDatabase m_db;
		bool m_openOk, m_dirtyState;
};

}
