#include "gsc_mysql.hpp"

#if COMPILE_MYSQL == 1

/*
	Had the problem, that a query failed but no mysql_errno() was set
	Reason: mysql_query() didnt even got executed, because the str was undefined
	So the function quittet with stackPushInt(0)
	Now its undefined, and i shall test it every time
*/

#include <mysql/mysql.h>
#include <pthread.h>

struct mysql_async_task
{
	mysql_async_task *prev;
	mysql_async_task *next;
	int id;
	MYSQL_RES *result;
	bool done;
	bool started;
	bool save;
	char query[COD2_MAX_STRINGLENGTH + 1];
};

struct mysql_async_connection
{
	mysql_async_connection *prev;
	mysql_async_connection *next;
	mysql_async_task* task;
	MYSQL *connection;
};

mysql_async_connection *first_async_connection = NULL;
mysql_async_task *first_async_task = NULL;
MYSQL *cod_mysql_connection = NULL;

void *mysql_async_execute_query(void *input_c) //cannot be called from gsc, is threaded.
{
	mysql_async_connection *c = (mysql_async_connection *) input_c;
	int i = 0;
	int lastquery = 0;
	bool goodquery = false;
	bool opened = false;
	mysql_async_task *q = c->task;
	while(q->query[i] != '\0')
	{
		if(q->query[i] == '\'' && i > 0 && q->query[i - 1] != '\\')
		{
			if(opened)
				opened = false;
			else
				opened = true;
		}
		if(q->query[i] == ';' && i > 0 && q->query[i - 1] != '\\' && !opened)
		{
			//subquery
			q->query[i] = '\0';
			int res = mysql_query(c->connection, &(q->query[lastquery]));
			q->query[i] = ';';
			if(res)
				goodquery = false;
			else
				goodquery = true;
			lastquery = i + 1;
			while(q->query[lastquery] != '\0' && (q->query[lastquery] == ' ' || q->query[lastquery] == '\t'))
				lastquery++;
		}
		i++;
	}
	if(i - lastquery > 5)
	{
		//this is a query
		int res = mysql_query(c->connection, &(q->query[lastquery]));
		if(res)
			goodquery = false;
		else
			goodquery = true;
	}
	if(goodquery && q->save)
		q->result = mysql_store_result(c->connection);
	c->task = NULL;
	q->done = true;
	return NULL;
}

void *mysql_async_query_handler(void* input_nothing) //is threaded after initialize
{
	static bool started = false;
	if(started)
	{
		printf("scriptengine> async handler already started. Returning\n");
		return NULL;
	}
	started = true;
	mysql_async_connection *c = first_async_connection;
	if(c == NULL)
	{
		printf("scriptengine> async handler started before any connection was initialized\n"); //this should never happen
		started = false;
		return NULL;
	}
	mysql_async_task *q;
	while(true)
	{
		q = first_async_task;
		c = first_async_connection;
		while(q != NULL)
		{
			if(!q->started)
			{
				while(c != NULL && c->task != NULL)
					c = c->next;
				if(c == NULL)
				{
					//out of free connections
					break;
				}
				q->started = true;
				c->task = q;
				pthread_t query_doer;
				int error = pthread_create(&query_doer, NULL, mysql_async_execute_query, c);
				if(error)
				{
					printf("error: %d\n", error);
					printf("Error detaching async handler thread\n");
					return NULL;
				}
				pthread_detach(query_doer);
				//std::thread async_query(mysql_async_execute_query, q, c);
				//async_query.detach();
				c = c->next;
			}
			q = q->next;
		}
		usleep(10000);
	}
	return NULL;
}

int mysql_async_query_initializer(char *sql, bool save) //cannot be called from gsc, helper function
{
	static int id = 0;
	id++;
	mysql_async_task *current = first_async_task;
	while(current != NULL && current->next != NULL)
		current = current->next;
	mysql_async_task *newtask = new mysql_async_task;
	newtask->id = id;
	strncpy(newtask->query, sql, COD2_MAX_STRINGLENGTH);
	newtask->prev = current;
	newtask->result = NULL;
	newtask->save = save;
	newtask->done = false;
	newtask->next = NULL;
	newtask->started = false;
	if(current != NULL)
		current->next = newtask;
	else
		first_async_task = newtask;
	stackPushInt(id);
	return id;
}

void gsc_mysql_async_create_query_nosave()
{
	char *sql;
	if ( ! stackGetParams("s", &sql))
	{
		printf("scriptengine> wrongs args for create_mysql_async_query(...);\n");
		stackPushUndefined();
		return;
	}
	int id = mysql_async_query_initializer(sql, false);
	stackPushInt(id);
	return;
}

void gsc_mysql_async_create_query()
{
	char *sql;
	if ( ! stackGetParams("s", &sql))
	{
		printf("scriptengine> wrongs args for create_mysql_async_query(...);\n");
		stackPushUndefined();
		return;
	}
	int id = mysql_async_query_initializer(sql, true);
	stackPushInt(id);
	return;
}

void gsc_mysql_async_getdone_list()
{
	mysql_async_task *current = first_async_task;
	stackPushArray();
	while(current != NULL)
	{
		if(current->done)
		{
			stackPushInt((int)current->id);
			stackPushArrayLast();
		}
		current = current->next;
	}
}

void gsc_mysql_async_getresult_and_free() //same as above, but takes the id of a function instead and returns 0 (not done), undefined (not found) or the mem address of result
{
	int id;
	if(!stackGetParams("i", &id))
	{
		printf("scriptengine> wrong args for mysql_async_getresult_and_free_id\n");
		stackPushUndefined();
		return;
	}
	mysql_async_task *c = first_async_task;
	if(c != NULL)
	{
		while(c != NULL && c->id != id)
			c = c->next;
	}
	if(c != NULL)
	{
		if(!c->done)
		{
			stackPushUndefined(); //not done yet
			return;
		}
		if(c->next != NULL)
			c->next->prev = c->prev;
		if(c->prev != NULL)
			c->prev->next = c->next;
		else
			first_async_task = c->next;
		if(c->save)
		{
			int ret = (int)c->result;
			stackPushInt(ret);
		}
		else
			stackPushInt(0);
		delete c;
		return;
	}
	else
	{
		printf("scriptengine> mysql async query id not found\n");
		stackPushUndefined();
		return;
	}
}

void gsc_mysql_async_initializer()//returns array with mysql connection handlers
{
	if(first_async_connection != NULL)
	{
		printf("scriptengine> Async mysql already initialized. Returning before adding additional connections\n");
		stackPushUndefined();
		return;
	}
	int port, connection_count;
	char *host, *user, *pass, *db;

	if ( ! stackGetParams("ssssii", &host, &user, &pass, &db, &port, &connection_count))
	{
		printf("scriptengine> wrongs args for mysql_async_initializer(...);\n");
		stackPushUndefined();
		return;
	}
	if(connection_count <= 0)
	{
		printf("Need a positive connection_count in mysql_async_initializer\n");
		stackPushUndefined();
		return;
	}
	int i;
	stackPushArray();
	mysql_async_connection *current = first_async_connection;
	for(i = 0; i < connection_count; i++)
	{
		mysql_async_connection *newconnection = new mysql_async_connection;
		newconnection->next = NULL;
		newconnection->connection = mysql_init(NULL);
		newconnection->connection = mysql_real_connect((MYSQL*)newconnection->connection, host, user, pass, db, port, NULL, 0);
		my_bool reconnect = true;
		mysql_options(newconnection->connection, MYSQL_OPT_RECONNECT, &reconnect);
		newconnection->task = NULL;
		if(current == NULL)
		{
			newconnection->prev = NULL;
			first_async_connection = newconnection;
		}
		else
		{
			while(current->next != NULL)
				current = current->next;
			current->next = newconnection;
			newconnection->prev = current;
		}
		current = newconnection;
		stackPushInt((int)newconnection->connection);
		stackPushArrayLast();
	}
	pthread_t async_handler;
	if(pthread_create(&async_handler, NULL, mysql_async_query_handler, NULL))
	{
		printf("Error detaching async handler thread\n");
		return;
	}
	pthread_detach(async_handler);
	//std::thread async_query(mysql_async_query_handler);
	//async_query.detach();
}

void gsc_mysql_init()
{
#if DEBUG_MYSQL
	printf("gsc_mysql_init()\n");
#endif
	MYSQL *my = mysql_init(NULL);
	stackPushInt((int) my);
}

void gsc_mysql_reuse_connection()
{
	if(cod_mysql_connection == NULL)
	{
		stackPushUndefined();
		return;
	}
	else
	{
		stackPushInt((int) cod_mysql_connection);
		return;
	}
}

void gsc_mysql_real_connect()
{
	int mysql, port;
	char *host, *user, *pass, *db;

	if ( ! stackGetParams("issssi", &mysql, &host, &user, &pass, &db, &port))
	{
		printf("scriptengine> wrongs args for mysql_real_connect(...);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_real_connect(mysql=%d, host=\"%s\", user=\"%s\", pass=\"%s\", db=\"%s\", port=%d)\n", mysql, host, user, pass, db, port);
#endif

	mysql = (int) mysql_real_connect((MYSQL *)mysql, host, user, pass, db, port, NULL, 0);
	my_bool reconnect = true;
	mysql_options((MYSQL*)mysql, MYSQL_OPT_RECONNECT, &reconnect);
	if(cod_mysql_connection == NULL)
		cod_mysql_connection = (MYSQL*) mysql;
	stackPushInt(mysql);
}

void gsc_mysql_close()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		printf("scriptengine> wrongs args for mysql_close(mysql);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_close(%d)\n", mysql);
#endif

	mysql_close((MYSQL *)mysql);
	stackPushInt(0);
}

void gsc_mysql_query()
{
	int mysql;
	char *sql;

	if ( ! stackGetParams("is", &mysql, &sql))
	{
		printf("scriptengine> wrongs args for mysql_query(...);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_query(%d, \"%s\")\n", mysql, sql);
#endif

	int ret = mysql_query((MYSQL *)mysql, sql);
	stackPushInt(ret);
}

void gsc_mysql_errno()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		printf("scriptengine> wrongs args for mysql_errno(mysql);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_errno(%d)\n", mysql);
#endif

	int ret = mysql_errno((MYSQL *)mysql);
	stackPushInt(ret);
}

void gsc_mysql_error()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		printf("scriptengine> wrongs args for mysql_error(mysql);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_error(%d)\n", mysql);
#endif

	char *ret = (char *)mysql_error((MYSQL *)mysql);
	stackPushString(ret);
}

void gsc_mysql_affected_rows()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		printf("scriptengine> wrongs args for mysql_affected_rows(mysql);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_affected_rows(%d)\n", mysql);
#endif

	int ret = mysql_affected_rows((MYSQL *)mysql);
	stackPushInt(ret);
}

void gsc_mysql_store_result()
{
	int mysql;

	if ( ! stackGetParams("i", &mysql))
	{
		printf("scriptengine> wrongs args for mysql_store_result(mysql);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_store_result(%d)\n", mysql);
#endif

	MYSQL_RES *result = mysql_store_result((MYSQL *)mysql);
	stackPushInt((int) result);
}

void gsc_mysql_num_rows()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		printf("scriptengine> wrongs args for mysql_num_rows(result);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_num_rows(result=%d)\n", result);
#endif

	int ret = mysql_num_rows((MYSQL_RES *)result);
	stackPushInt(ret);
}

void gsc_mysql_num_fields()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		printf("scriptengine> wrongs args for mysql_num_fields(result);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_num_fields(result=%d)\n", result);
#endif

	int ret = mysql_num_fields((MYSQL_RES *)result);
	stackPushInt(ret);
}

void gsc_mysql_field_seek()
{
	int result;
	int offset;

	if ( ! stackGetParams("ii", &result, &offset))
	{
		printf("scriptengine> wrongs args for mysql_field_seek(result, offset);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_field_seek(result=%d, offset=%d)\n", result, offset);
#endif

	int ret = mysql_field_seek((MYSQL_RES *)result, offset);
	stackPushInt(ret);
}

void gsc_mysql_fetch_field()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		printf("scriptengine> wrongs args for mysql_fetch_field(result);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_fetch_field(result=%d)\n", result);
#endif

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
		printf("scriptengine> wrongs args for mysql_fetch_row(result);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_fetch_row(result=%d)\n", result);
#endif

	MYSQL_ROW row = mysql_fetch_row((MYSQL_RES *)result);
	if (!row)
	{
#if DEBUG_MYSQL
		printf("row == NULL\n");
#endif
		stackPushUndefined();
		return;
	}

	stackPushArray();

	int numfields = mysql_num_fields((MYSQL_RES *)result);
	for (int i=0; i<numfields; i++)
	{
		if (row[i] == NULL)
			stackPushUndefined();
		else
			stackPushString(row[i]);

#if DEBUG_MYSQL
		printf("row == \"%s\"\n", row[i]);
#endif
		stackPushArrayLast();
	}
}

void gsc_mysql_free_result()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		printf("scriptengine> wrongs args for mysql_free_result(result);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_free_result(result=%d)\n", result);
#endif
	if(result == 0)
	{
		printf("scriptengine> Error in mysql_free_result: input is a NULL-pointer\n");
		stackPushUndefined();
		return;
	}
	mysql_free_result((MYSQL_RES *)result);
	stackPushUndefined();
}

void gsc_mysql_real_escape_string()
{
	int mysql;
	char *str;

	if ( ! stackGetParams("is", &mysql, &str))
	{
		printf("scriptengine> wrongs args for mysql_real_escape_string(...);\n");
		stackPushUndefined();
		return;
	}
#if DEBUG_MYSQL
	printf("gsc_mysql_real_escape_string(%d, \"%s\")\n", mysql, str);
#endif

	char *to = (char *) malloc(strlen(str) * 2 + 1);
	mysql_real_escape_string((MYSQL *)mysql, to, str, strlen(str));
	stackPushString(to);
	free(to);
}

#endif
