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

#include "config.hpp" // DEBUG_MYSQL etc.
#include "functions.hpp"
#include "gsc_player.hpp"
#include "gsc_mysql.hpp"
#include "gsc_memory.hpp"
#include "gsc_utils.hpp"
#include "cracking.hpp"

#ifdef EXTRA_INCLUDES_INC
#include "extra_includes.hpp"
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

typedef struct
{
	void *offsetData;
	int type;
} aStackElement;

int getStack();

void stackError(char *format, ...);

int stackGetParamInt(int param, int *value);
int stackGetParamString(int param, char **value);
int stackGetParamVector(int param, float value[3]);
int stackGetParamFloat(int param, float *value);
int stackGetNumberOfParams();
int stackGetParamType(int param);
char *stackGetParamTypeAsString(int param);
int stackGetParams(char *params, ...);

int stackPushUndefined();
int stackPushInt(int ret);
int stackPushVector(float *ret);
int stackPushFloat(float ret);
int stackPushString(char *toPush);
int stackPushEntity(int arg);
int stackPushArray();
int stackPushArrayLast();

// functions
typedef void (*Scr_FunctionCall)();

typedef struct
{
	const char *name;
	Scr_FunctionCall call;
	int developer;
} Scr_Function;

typedef Scr_FunctionCall (*Scr_GetFunction_t)(const char **fname, int *fdev);

Scr_FunctionCall Scr_GetCustomFunction(const char **fname, int *fdev); // could be made obsolete to remove the cracking_hook_call()-stuff

// methods
typedef void (*Scr_MethodCall)(int);

typedef struct
{
	const char* name;
	Scr_MethodCall call;
	int developer;
} Scr_Method;

typedef Scr_MethodCall (*Scr_GetMethod_t)(const char**, int*);

Scr_MethodCall Scr_GetCustomMethod(const char **fname, int *fdev); // could be made obsolete to remove the cracking_hook_call()-stuff

#ifdef __cplusplus
}
#endif

#endif
