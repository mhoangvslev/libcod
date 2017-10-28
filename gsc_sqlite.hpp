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

#ifdef __cplusplus
}
#endif

#endif
