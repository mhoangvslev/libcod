#include "gsc_mysql.hpp"

#if COMPILE_MYSQL == 1

#include <mysql/mysql.h>

MYSQL *mysql_connection = NULL;

void gsc_mysql_initialize()
{
	char *host, *user, *pass, *db;
	int port;

	if ( ! stackGetParams("ssssi", &host, &user, &pass, &db, &port))
	{
		stackError("gsc_mysql_initialize() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		MYSQL *my = mysql_init(NULL);
		my_bool reconnect = true;
		mysql_options(my, MYSQL_OPT_RECONNECT, &reconnect);

		if (!mysql_real_connect(my, host, user, pass, db, port, NULL, 0))
		{
			stackError("gsc_mysql_initialize() failed to initialize synchronous mysql connection!");
			stackPushUndefined();
			return;
		}

		mysql_connection = (MYSQL*)my;
	}
	else
		Com_DPrintf("gsc_mysql_initialize() synchronous connection already initialized.\n");

	stackPushInt((int)mysql_connection);
}

void gsc_mysql_close()
{
	if (mysql_connection != NULL)
	{
		mysql_close(mysql_connection);
		stackPushInt(1);
	}
	else
		stackPushInt(0);
}

void gsc_mysql_query()
{
	char *query;

	if ( ! stackGetParams("s", &query))
	{
		stackError("gsc_mysql_query() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_query() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_query(mysql_connection, query));
}

void gsc_mysql_errno()
{
	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_errno() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_errno(mysql_connection));
}

void gsc_mysql_error()
{
	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_error() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushString((char *)mysql_error(mysql_connection));
}

void gsc_mysql_affected_rows()
{
	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_affected_rows() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_affected_rows(mysql_connection));
}

void gsc_mysql_store_result()
{
	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_store_result() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	MYSQL_RES *result = mysql_store_result(mysql_connection);
	stackPushInt((int)result);
}

void gsc_mysql_num_rows()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_num_rows() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_num_rows() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	int ret = mysql_num_rows((MYSQL_RES *)result);
	stackPushInt(ret);
}

void gsc_mysql_num_fields()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_num_fields() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_num_fields() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	int ret = mysql_num_fields((MYSQL_RES *)result);
	stackPushInt(ret);
}

void gsc_mysql_field_seek()
{
	int result;
	int offset;

	if ( ! stackGetParams("ii", &result, &offset))
	{
		stackError("gsc_mysql_field_seek() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_field_seek() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	int ret = mysql_field_seek((MYSQL_RES *)result, offset);
	stackPushInt(ret);
}

void gsc_mysql_fetch_field()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_fetch_field() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_fetch_field() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	MYSQL_FIELD *field = mysql_fetch_field((MYSQL_RES *)result);

	if (field == NULL)
	{
		stackPushUndefined();
		return;
	}

	char *ret = field->name;
	stackPushString(ret);
}

void gsc_mysql_fetch_row()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_fetch_row() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_fetch_row() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	MYSQL_ROW row = mysql_fetch_row((MYSQL_RES *)result);

	if (!row)
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();

	int numfields = mysql_num_fields((MYSQL_RES *)result);

	for (int i = 0; i < numfields; i++)
	{
		if (row[i] == NULL)
			stackPushUndefined();
		else
			stackPushString(row[i]);

		stackPushArrayLast();
	}
}

void gsc_mysql_free_result()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_free_result() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_free_result() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	mysql_free_result((MYSQL_RES *)result);
	stackPushInt(1);
}

void gsc_mysql_real_escape_string()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_mysql_real_escape_string() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_real_escape_string() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	char *to = (char *)malloc(strlen(str) * 2 + 1);
	mysql_real_escape_string(mysql_connection, to, str, strlen(str));
	stackPushString(to);
	free(to);
}

#endif
