#include "gsc_sqlite.hpp"

#if COMPILE_SQLITE == 1

#include <sqlite3.h>
#include <pthread.h>

#define INVALID_ENTITY -1
#define INVALID_STATE 0

#define MAX_SQLITE_FIELDS 256
#define MAX_SQLITE_ROWS 256
#define MAX_SQLITE_TASKS 512

enum
{
	INT_VALUE,
	FLOAT_VALUE,
	STRING_VALUE,
	VECTOR_VALUE,
	OBJECT_VALUE
};

struct async_sqlite_task
{
	async_sqlite_task *prev;
	async_sqlite_task *next;
	sqlite3 *db;
	sqlite3_stmt *statement;
	char query[COD2_MAX_STRINGLENGTH];
	unsigned int row[MAX_SQLITE_FIELDS][MAX_SQLITE_ROWS];
	int fields_size;
	int rows_size;
	int callback;
	bool done;
	bool save;
	bool error;
	char errorMessage[COD2_MAX_STRINGLENGTH];
	unsigned int levelId;
	bool hasargument;
	int valueType;
	int intValue;
	float floatValue;
	char stringValue[COD2_MAX_STRINGLENGTH];
	vec3_t vectorValue;
	unsigned int objectValue;
	int entityNum;
	int entityState;
};

async_sqlite_task *first_async_sqlite_task = NULL;
int async_sqlite_initialized = 0;

void *async_sqlite_query_handler(void* dummy)
{
	while(1)
	{
		async_sqlite_task *current = first_async_sqlite_task;

		while (current != NULL)
		{
			async_sqlite_task *task = current;
			current = current->next;

			if (!task->done)
			{
				int rc = sqlite3_prepare_v2(task->db, task->query, COD2_MAX_STRINGLENGTH, &task->statement, 0);

				if (rc == SQLITE_OK)
				{
					if (task->save && task->callback)
						task->fields_size = 0;

					int rs = sqlite3_step(task->statement);

					while (rs != SQLITE_DONE)
					{
						if (rs < SQLITE_NOTICE && rs != SQLITE_BUSY)
						{
							task->error = true;

							strncpy(task->errorMessage, sqlite3_errmsg(task->db), COD2_MAX_STRINGLENGTH - 1);
							task->errorMessage[COD2_MAX_STRINGLENGTH - 1] = '\0';
							sqlite3_finalize(task->statement);
							break;
						}

						if (rs == SQLITE_ROW && task->save && task->callback)
						{
							if (task->fields_size > MAX_SQLITE_FIELDS - 1)
								continue;

							task->rows_size = 0;

							for (int i = 0; i < sqlite3_column_count(task->statement); i++)
							{
								if (task->rows_size > MAX_SQLITE_ROWS - 1)
									continue;

								task->row[task->fields_size][task->rows_size] = SL_GetString((char *)sqlite3_column_text(task->statement, i), 0);
								task->rows_size++;
							}

							task->fields_size++;
						}

						rs = sqlite3_step(task->statement);
					}
				}
				else
				{
					if (rc != SQLITE_BUSY)
					{
						task->error = true;

						strncpy(task->errorMessage, sqlite3_errmsg(task->db), COD2_MAX_STRINGLENGTH - 1);
						task->errorMessage[COD2_MAX_STRINGLENGTH - 1] = '\0';
					}
				}

				if (rc != SQLITE_BUSY)
					task->done = true;
			}
		}

		usleep(10000);
	}

	return NULL;
}

void gsc_async_sqlite_initialize()
{
	if (!async_sqlite_initialized)
	{
		pthread_t async_handler;

		if (pthread_create(&async_handler, NULL, async_sqlite_query_handler, NULL) != 0)
		{
			stackError("gsc_async_sqlite_initialize() error creating async handler thread!");
			stackPushUndefined();
			return;
		}

		if (pthread_detach(async_handler) != 0)
		{
			stackError("gsc_async_sqlite_initialize() error detaching async handler thread!");
			stackPushUndefined();
			return;
		}

		async_sqlite_initialized = 1;
	}
	else
		Com_DPrintf("gsc_async_sqlite_initialize() async handler already initialized.\n");

	stackPushInt(async_sqlite_initialized);
}

void gsc_async_sqlite_create_query()
{
	int db;
	char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_async_sqlite_create_query() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!async_sqlite_initialized)
	{
		stackError("gsc_async_sqlite_create_query() async handler has not been initialized");
		stackPushUndefined();
		return;
	}

	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (task_count > MAX_SQLITE_TASKS - 1)
		{
			stackError("gsc_async_sqlite_create_query() exceeded async task limit");
			stackPushUndefined();
			return;
		}

		current = current->next;
		task_count++;
	}

	async_sqlite_task *newtask = new async_sqlite_task;

	newtask->prev = current;
	newtask->next = NULL;

	newtask->db = (sqlite3 *)db;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	int callback;

	if (!stackGetParamFunction(2, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = true;
	newtask->error = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;
	newtask->entityNum = INVALID_ENTITY;
	newtask->entityState = INVALID_STATE;

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(3, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(3, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(3, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(3, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(3, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_sqlite_task = newtask;

	stackPushInt(1);
}

void gsc_async_sqlite_create_query_nosave()
{
	int db;
	char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_async_sqlite_create_query_nosave() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!async_sqlite_initialized)
	{
		stackError("gsc_async_sqlite_create_query_nosave() async handler has not been initialized");
		stackPushUndefined();
		return;
	}

	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (task_count > MAX_SQLITE_TASKS - 1)
		{
			stackError("gsc_async_sqlite_create_query_nosave() exceeded async task limit");
			stackPushUndefined();
			return;
		}

		current = current->next;
		task_count++;
	}

	async_sqlite_task *newtask = new async_sqlite_task;

	newtask->prev = current;
	newtask->next = NULL;

	newtask->db = (sqlite3 *)db;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	int callback;

	if (!stackGetParamFunction(2, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = false;
	newtask->error = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;
	newtask->entityNum = INVALID_ENTITY;
	newtask->entityState = INVALID_STATE;

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(3, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(3, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(3, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(3, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(3, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_sqlite_task = newtask;

	stackPushInt(1);
}

void gsc_async_sqlite_create_entity_query(int entid)
{
	int db;
	char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_async_sqlite_create_entity_query() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!async_sqlite_initialized)
	{
		stackError("gsc_async_sqlite_create_entity_query() async handler has not been initialized");
		stackPushUndefined();
		return;
	}

	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (task_count > MAX_SQLITE_TASKS - 1)
		{
			stackError("gsc_async_sqlite_create_entity_query() exceeded async task limit");
			stackPushUndefined();
			return;
		}

		current = current->next;
		task_count++;
	}

	async_sqlite_task *newtask = new async_sqlite_task;

	newtask->prev = current;
	newtask->next = NULL;

	newtask->db = (sqlite3 *)db;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	int callback;

	if (!stackGetParamFunction(2, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = true;
	newtask->error = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;
	newtask->entityNum = entid;
	newtask->entityState = *(int *)(G_ENTITY(newtask->entityNum) + 1);

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(3, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(3, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(3, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(3, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(3, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_sqlite_task = newtask;

	stackPushInt(1);
}

void gsc_async_sqlite_create_entity_query_nosave(int entid)
{
	int db;
	char *query;

	if ( ! stackGetParams("is", &db, &query))
	{
		stackError("gsc_async_sqlite_create_entity_query_nosave() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!async_sqlite_initialized)
	{
		stackError("gsc_async_sqlite_create_entity_query_nosave() async handler has not been initialized");
		stackPushUndefined();
		return;
	}

	async_sqlite_task *current = first_async_sqlite_task;

	int task_count = 0;

	while (current != NULL && current->next != NULL)
	{
		if (task_count > MAX_SQLITE_TASKS - 1)
		{
			stackError("gsc_async_sqlite_create_entity_query_nosave() exceeded async task limit");
			stackPushUndefined();
			return;
		}

		current = current->next;
		task_count++;
	}

	async_sqlite_task *newtask = new async_sqlite_task;

	newtask->prev = current;
	newtask->next = NULL;

	newtask->db = (sqlite3 *)db;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	int callback;

	if (!stackGetParamFunction(2, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = false;
	newtask->error = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;
	newtask->entityNum = entid;
	newtask->entityState = *(int *)(G_ENTITY(newtask->entityNum) + 1);

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(3, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(3, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(3, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(3, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(3, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_sqlite_task = newtask;

	stackPushInt(1);
}

void gsc_async_sqlite_checkdone()
{
	async_sqlite_task *current = first_async_sqlite_task;

	while (current != NULL)
	{
		async_sqlite_task *task = current;
		current = current->next;

		if (task->done)
		{
			if (Scr_IsSystemActive() && (scrVarPub.levelId == task->levelId))
			{
				if (!task->error)
				{
					if (task->save && task->callback)
					{
						if (task->entityNum != INVALID_ENTITY)
						{
							if (task->entityState != INVALID_STATE)
							{
								int state = *(int *)(G_ENTITY(task->entityNum) + 1);

								if (state != INVALID_STATE && state == task->entityState)
								{
									if (task->hasargument)
									{
										switch(task->valueType)
										{
										case INT_VALUE:
											stackPushInt(task->intValue);
											break;

										case FLOAT_VALUE:
											stackPushFloat(task->floatValue);
											break;

										case STRING_VALUE:
											stackPushString(task->stringValue);
											break;

										case VECTOR_VALUE:
											stackPushVector(task->vectorValue);
											break;

										case OBJECT_VALUE:
											stackPushObject(task->objectValue);
											break;

										default:
											stackPushUndefined();
											break;
										}
									}

									stackPushArray();

									for (int i = 0; i < task->fields_size; i++)
									{
										stackPushArray();

										for (int x = 0; x < task->rows_size; x++)
										{
											stackPushString(SL_ConvertToString(task->row[i][x]));
											SL_RemoveRefToString(task->row[i][x]);
											stackPushArrayLast();
										}

										stackPushArrayLast();
									}

									short ret = Scr_ExecEntThread(G_ENTITY(task->entityNum), task->callback, task->save + task->hasargument);
									Scr_FreeThread(ret);
								}
							}
						}
						else
						{
							if (task->hasargument)
							{
								switch(task->valueType)
								{
								case INT_VALUE:
									stackPushInt(task->intValue);
									break;

								case FLOAT_VALUE:
									stackPushFloat(task->floatValue);
									break;

								case STRING_VALUE:
									stackPushString(task->stringValue);
									break;

								case VECTOR_VALUE:
									stackPushVector(task->vectorValue);
									break;

								default:
									stackPushUndefined();
									break;
								}
							}

							stackPushArray();

							for (int i = 0; i < task->fields_size; i++)
							{
								stackPushArray();

								for (int x = 0; x < task->rows_size; x++)
								{
									stackPushString(SL_ConvertToString(task->row[i][x]));
									SL_RemoveRefToString(task->row[i][x]);
									stackPushArrayLast();
								}

								stackPushArrayLast();
							}

							short ret = Scr_ExecThread(task->callback, task->save + task->hasargument);
							Scr_FreeThread(ret);
						}
					}
				}
				else
					stackError("gsc_async_sqlite_checkdone() query error in '%s' - '%s'", task->query, task->errorMessage);
			}

			if (task->next != NULL)
				task->next->prev = task->prev;

			if (task->prev != NULL)
				task->prev->next = task->next;
			else
				first_async_sqlite_task = task->next;

			if (!task->error)
				sqlite3_finalize(task->statement);

			delete task;
		}
	}
}

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

	int rs = sqlite3_step(statement);

	while (rs != SQLITE_DONE)
	{
		if (rs < SQLITE_NOTICE)
		{
			stackError("gsc_sqlite_query() failed to execute query: %s", sqlite3_errmsg((sqlite3 *)db));
			stackPushUndefined();
			sqlite3_finalize(statement);
			return;
		}

		if (rs == SQLITE_ROW)
		{
			stackPushArray();

			for (int i = 0; i < sqlite3_column_count(statement); i++)
			{
				stackPushString((char *)sqlite3_column_text(statement, i));
				stackPushArrayLast();
			}

			stackPushArrayLast();
		}

		rs = sqlite3_step(statement);
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
