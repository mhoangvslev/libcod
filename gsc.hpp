#ifndef _GSC_HPP_
#define _GSC_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#define COD2_1_0 210
#define COD2_1_2 212
#define COD2_1_3 213

#define COD2_MAX_STRINGLENGTH 1024

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h> // dlcall
#include <stdarg.h> // va_args

#include "config.hpp"
#include "cracking.hpp"
#include "functions.hpp"

#if COMPILE_BOTS == 1
#include "gsc_bots.hpp"
#endif

#if COMPILE_EXEC == 1
#include "gsc_exec.hpp"
#endif

#if COMPILE_MEMORY == 1
#include "gsc_memory.hpp"
#endif

#if COMPILE_MYSQL_DEFAULT == 1
#include "gsc_mysql.hpp"
#endif

#if COMPILE_MYSQL_VORON == 1
#include "gsc_mysql_voron.hpp"
#endif

#if COMPILE_PLAYER == 1
#include "gsc_player.hpp"
#endif

#if COMPILE_UTILS == 1
#include "gsc_utils.hpp"
#endif

#if COMPILE_WEAPONS == 1
#include "gsc_weapons.hpp"
#endif

#ifdef EXTRA_INCLUDES_INC
#include "extra/includes.hpp"
#endif

#define STACK_UNDEFINED 0
#define STACK_OBJECT 1
#define STACK_STRING 2
#define STACK_LOCALIZED_STRING 3
#define STACK_VECTOR 4
#define STACK_FLOAT 5
#define STACK_INT 6
#define STACK_CODEPOS 7
#define STACK_PRECODEPOS 8
#define STACK_FUNCTION 9
#define STACK_STACK 10
#define STACK_ANIMATION 11
#define STACK_DEVELOPER_CODEPOS 12
#define STACK_INCLUDE_CODEPOS 13
#define STACK_THREAD_LIST 14
#define STACK_THREAD_1 15
#define STACK_THREAD_2 16
#define STACK_THREAD_3 17
#define STACK_THREAD_4 18
#define STACK_STRUCT 19
#define STACK_REMOVED_ENTITY 20
#define STACK_ENTITY 21
#define STACK_ARRAY 22
#define STACK_REMOVED_THREAD 23

#if COD_VERSION == COD2_1_0
static const int varpub_offset = 0x08394000;
#elif COD_VERSION == COD2_1_2
static const int varpub_offset = 0x08396480;
#elif COD_VERSION == COD2_1_3
static const int varpub_offset = 0x08397500;
#endif

#if COD_VERSION == COD2_1_0
static const int vmpub_offset = 0x083D7600;
#elif COD_VERSION == COD2_1_2
static const int vmpub_offset = 0x083D7A00;
#elif COD_VERSION == COD2_1_3
static const int vmpub_offset = 0x083D8A80;
#endif

#define scrVarPub (*((scrVarPub_t*)( varpub_offset )))
#define scrVmPub (*((scrVmPub_t*)( vmpub_offset )))

void stackError(char *format, ...);

char *stackGetParamTypeAsString(int param);

int stackGetParamInt(int param, int *value);
int stackGetParamFunction(int param, int *value);
int stackGetParamString(int param, char **value);
int stackGetParamVector(int param, float value[3]);
int stackGetParamFloat(int param, float *value);
int stackGetParamType(int param);
int stackGetParams(char *params, ...);

#define stackPushUndefined Scr_AddUndefined
#define stackPushInt Scr_AddInt
#define stackPushFloat Scr_AddFloat
#define stackPushString Scr_AddString
#define stackPushVector Scr_AddVector
#define stackPushEntity Scr_AddEntity
#define stackPushArray Scr_MakeArray
#define stackPushArrayLast Scr_AddArray

xfunction_t Scr_GetCustomFunction(const char **fname, int *fdev);
xmethod_t Scr_GetCustomMethod(const char **fname, int *fdev);

#ifdef __cplusplus
}
#endif

#endif
