#ifndef _GSC_SQLITE_HPP_
#define _GSC_SQLITE_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* gsc functions */
#include "gsc.hpp"

void gsc_sqlite_open();
void gsc_sqlite_query();
void gsc_sqlite_close();

void gsc_async_sqlite_initialize();
void gsc_async_sqlite_create_query();
void gsc_async_sqlite_create_query_nosave();
void gsc_async_sqlite_checkdone();

void gsc_async_sqlite_create_entity_query(int entid);
void gsc_async_sqlite_create_entity_query_nosave(int entid);

#ifdef __cplusplus
}
#endif

#endif
