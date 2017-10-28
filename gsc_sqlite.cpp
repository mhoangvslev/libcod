#include "gsc_sqlite.hpp"

#if COMPILE_SQLITE == 1

#include <sqlite3.h>

void gsc_sqlite_open()
{
	char *database;

	if ( ! stackGetParams("s", &database))
	{
		stackError("gsc_sqlite_open() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	sqlite3 *db;

	int rc = sqlite3_open(database, &db);

	if (rc != SQLITE_OK)
	{
		stackError("gsc_sqlite_open() cannot open database: %s", sqlite3_errmsg(db));
		stackPushUndefined();
		return;
	}

	stackPushInt((int)db);
}

void gsc_sqlite_query()
{
	int db;
	char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_sqlite_query() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	sqlite3_stmt *statement;

	int rc = sqlite3_prepare_v2((sqlite3 *)db, query, COD2_MAX_STRINGLENGTH, &statement, 0);

	if (rc != SQLITE_OK)
	{
		stackError("gsc_sqlite_query() failed to fetch query data: %s", sqlite3_errmsg((sqlite3 *)db));
		stackPushUndefined();
		return;
	}

	stackPushArray();

	while (sqlite3_step(statement) == SQLITE_ROW)
	{
		stackPushArray();

		for (int i = 0; i < sqlite3_column_count(statement); i++)
		{
			stackPushString((char *)sqlite3_column_text(statement, i));
			stackPushArrayLast();
		}

		stackPushArrayLast();
	}

	sqlite3_finalize(statement);
}

void gsc_sqlite_close()
{
	int db;

	if ( ! stackGetParams("i", &db))
	{
		stackError("gsc_sqlite_close() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int rc = sqlite3_close((sqlite3 *)db);

	if (rc != SQLITE_OK)
	{
		stackError("gsc_sqlite_close() cannot close database: %s", sqlite3_errmsg((sqlite3 *)db));
		stackPushUndefined();
		return;
	}

	stackPushInt(1);
}

#endif
