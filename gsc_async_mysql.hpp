#ifndef _GSC_ASYNC_MYSQL_HPP_
#define _GSC_ASYNC_MYSQL_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

/* offsetof */
#include <stddef.h>


void gsc_async_mysql_initialize();
void gsc_async_mysql_close();
void gsc_async_mysql_create_query();
void gsc_async_mysql_create_query_nosave();
void gsc_async_mysql_checkdone();
void gsc_async_mysql_errno();
void gsc_async_mysql_error();
void gsc_async_mysql_affected_rows();
void gsc_async_mysql_num_rows();
void gsc_async_mysql_num_fields();
void gsc_async_mysql_field_seek();
void gsc_async_mysql_fetch_field();
void gsc_async_mysql_fetch_row();
void gsc_async_mysql_free_task();
void gsc_async_mysql_real_escape_string();

#ifdef __cplusplus
}
#endif

#endif