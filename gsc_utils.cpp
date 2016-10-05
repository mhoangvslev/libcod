#include "gsc_utils.hpp"

#if COMPILE_UTILS == 1

#include <dirent.h> // dir stuff
#include <ctype.h> // toupper

//thanks to riicchhaarrd/php
unsigned short Scr_GetArray(int index)
{
	if (index >= stackGetNumberOfParams())
	{
		stackError("Scr_GetArray() one parameter is required");
		return 0;
	}

	int stack = getStack();
	int base = *(int*)(stack - 8 * index);
	int vartype = *(int*)(base + 4);

	if (vartype == STACK_OBJECT)
		return *(unsigned short*)base;

	stackError("Scr_GetArray() the parameter must be an array");
	return 0;
}

void gsc_utils_getarraykeys()
{
	unsigned short arrIndex = Scr_GetArray(0);
	stackPushArray();

	if (arrIndex == 0)
		return; // we didn't find a valid array

	unsigned short i;
	for(i = GetNextVariable(arrIndex); i != 0;)
	{
		stackPushString(SL_ConvertToString(GetVariableName(i)));
		stackPushArrayLast();
		i = GetNextVariable(i);
	}
}

int stackPrintParam(int param)
{
	if (param >= stackGetNumberOfParams())
		return 0;

	switch (stackGetParamType(param))
	{
	case STACK_STRING:
		char *str;
		stackGetParamString(param, &str); // no error checking, since we know it's a string
		printf("%s", str);
		return 1;

	case STACK_VECTOR:
		float vec[3];
		stackGetParamVector(param, vec);
		printf("(%.2f, %.2f, %.2f)", vec[0], vec[1], vec[2]);
		return 1;

	case STACK_FLOAT:
		float tmp_float;
		stackGetParamFloat(param, &tmp_float);
		printf("%.3f", tmp_float); // need a way to define precision
		return 1;

	case STACK_INT:
		int tmp_int;
		stackGetParamInt(param, &tmp_int);
		printf("%d", tmp_int);
		return 1;
	}
	printf("(%s)", stackGetParamTypeAsString(param));
	return 0;
}

void gsc_utils_printf()
{
	char *str;
	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_printf() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	int param = 1; // maps to first %
	int len = strlen(str);

	for (int i = 0; i < len; i++)
	{
		if (str[i] == '%')
		{
			if(str[i + 1] == '%')
			{
				putchar('%');
				i++;
			}
			else
				stackPrintParam(param++);
		}
		else
			putchar(str[i]);
	}

	stackPushInt(1);
}

void gsc_utils_sprintf()
{
	char result[COD2_MAX_STRINGLENGTH];
	char *str;
	if (!stackGetParams("s", &str))
	{
		stackError("gsc_utils_sprintf() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int param = 1; // maps to first %
	int len = strlen(str);
	int num = 0;
	for (int i = 0; i < len; i++)
	{
		if (str[i] == '%')
		{
			if(str[i + 1] == '%')
			{
				result[num++] = '%';
				i++;
			}
			else
			{
				if(param >= stackGetNumberOfParams())
					continue;

				switch (stackGetParamType(param))
				{
				case STACK_STRING:
					char *tmp_str;
					stackGetParamString(param, &tmp_str); // no error checking, since we know it's a string
					num += sprintf(&(result[num]), "%s", tmp_str);
					break;

				case STACK_VECTOR:
					float vec[3];
					stackGetParamVector(param, vec);
					num += sprintf(&(result[num]), "(%.2f, %.2f, %.2f)", vec[0], vec[1], vec[2]);
					break;
				case STACK_FLOAT:

					float tmp_float;
					stackGetParamFloat(param, &tmp_float);
					num += sprintf(&(result[num]), "%.3f", tmp_float); // need a way to define precision
					break;

				case STACK_INT:
					int tmp_int;
					stackGetParamInt(param, &tmp_int);
					num += sprintf(&(result[num]), "%d", tmp_int);
					break;
				}
				param++;
			}
		}
		else
			result[num++] = str[i];
	}
	result[num] = '\0';
	stackPushString(result);
}

void gsc_utils_getAscii()
{
	char *str;
	if ( ! stackGetParams("s", &str) || strlen(str) == 0)
	{
		stackError("gsc_utils_getAscii() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushInt(str[0]);
}

void gsc_utils_toupper()
{
	char *str;
	int offset = 0;
	int len = 0;
	if ( ! stackGetParams("s", &str) || strlen(str) == 0)
	{
		stackError("gsc_utils_toupper() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackGetParamInt(1, &offset);
	if(offset < 0)
		offset = 0;
	if(!stackGetParamInt(2, &len) || len == 0)
		len = strlen(str);
	if((len - offset) > int(strlen(str)))
		len = strlen(str) - offset;
	if(len <= 0)
	{
		stackPushString("");
		return;
	}

	int maxlen = strlen(str);
	char result[maxlen+1];
	strcpy(result, str);

	for (int i = offset; i < len; i++)
	{
		result[i] = toupper(str[i]);
	}
	result[maxlen] = '\0';

	stackPushString(result);
}

void gsc_utils_system()
{
	char *cmd;
	if ( ! stackGetParams("s",  &cmd))
	{
		stackError("gsc_utils_system() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	setenv("LD_PRELOAD", "", 1); // dont inherit lib of parent
	stackPushInt( system(cmd) );
}

// http://stackoverflow.com/questions/1583234/c-system-function-how-to-collect-the-output-of-the-issued-command
// Calling function must free the returned result.
char* exec(const char* command)
{
	FILE* fp;
	char* line = NULL;
	// Following initialization is equivalent to char* result = ""; and just
	// initializes result to an empty string, only it works with
	// -Werror=write-strings and is so much less clear.
	char* result = (char*) calloc(1, 1);
	size_t len = 0;

	fflush(NULL);
	fp = popen(command, "r");
	if (fp == NULL)
	{
		Com_DPrintf("exec() cannot execute command:\n%s\n", command);
		free(result);
		return NULL;
	}

	while(getline(&line, &len, fp) != -1)
	{
		// +1 below to allow room for null terminator.
		result = (char*) realloc(result, strlen(result) + strlen(line) + 1);
		// +1 below so we copy the final null terminator.
		strncpy(result + strlen(result), line, strlen(line) + 1);
		free(line);
		line = NULL;
	}

	fflush(fp);
	if (pclose(fp) != 0)
	{
		Com_DPrintf("exec() cannot close stream %i\n", fp);
		free(result);
		return NULL;
	}

	return result;
}

void gsc_utils_execute()   // Returns complete command output as a string
{
	char *cmd;
	if ( ! stackGetParams("s",  &cmd))
	{
		stackError("gsc_utils_execute() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	setenv("LD_PRELOAD", "", 1); // dont inherit lib of parent
	char *result = exec(cmd);
	if (result == NULL)
		stackPushUndefined();
	else
	{
		stackPushString(result);
		free(result);
	}
}

void gsc_utils_exponent()
{
	float basis;
	float exponent;
	if ( ! stackGetParams("ff", &basis, &exponent))
	{
		stackError("gsc_utils_exponent() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushFloat( pow(basis, exponent) );
}

void gsc_utils_file_link()
{
	char *source, *dest;
	if ( ! stackGetParams("ss",  &source, &dest))
	{
		stackError("gsc_utils_file_link() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	char cmd[COD2_MAX_STRINGLENGTH];
	setenv("LD_PRELOAD", "", 1); // dont inherit lib of parent
	snprintf(cmd, sizeof(cmd), "ln -sfn %s %s", source, dest);
	int link_failed = system(cmd);
	stackPushInt( link_failed ); // 0 == success
}

void gsc_utils_file_unlink()
{
	char *file;
	if ( ! stackGetParams("s",  &file))
	{
		stackError("gsc_utils_file_unlink() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushInt( unlink(file) ); // 0 == success
}

void gsc_utils_file_exists()
{
	char *filename;
	if ( ! stackGetParams("s", &filename))
	{
		stackError("gsc_utils_file_exists() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushInt( ! (access(filename, F_OK) == -1) );
}

void gsc_utils_FS_LoadDir()
{
	char *path, *dir;
	if ( ! stackGetParams("ss", &path, &dir))
	{
		stackError("gsc_utils_FS_LoadDir() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushInt( FS_LoadDir(path, dir) );
}

void gsc_utils_getType()
{
	if (stackGetNumberOfParams() == 0)
	{
		stackError("gsc_utils_getType() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushString( stackGetParamTypeAsString(0) );
}

void gsc_utils_stringToFloat()
{
	char *str;
	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_stringToFloat() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushFloat( atof(str) );
}

// rundll("print.so", "test_print")
void gsc_utils_rundll()
{
	char *arg_library, *arg_function;
	if ( ! stackGetParams("ss", &arg_library, &arg_function))
	{
		stackError("gsc_utils_rundll() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	Com_DPrintf("lib=%s func=%s\n", arg_library, arg_function);
	//void *handle = dlopen(arg_library, RTLD_GLOBAL); // crashes
	//void *handle = dlopen(arg_library, RTLD_LOCAL); // crashes
	//void *handle = dlopen(arg_library, RTLD_NOW); // crashes
	void *handle = dlopen(arg_library, RTLD_LAZY);
	if ( ! handle)
	{
		stackError("dlopen(\"%s\") failed", arg_library);
		stackPushInt(0);
		return;
	}
	Com_DPrintf("dlopen(\"%s\") returned: %.8x\n", arg_library, (unsigned int)handle);
	void (*func)();
	//*((void *)&func) = dlsym(handle, arg_function);
	*(int *)&func = (int)dlsym(handle, arg_function);
	if (!func)
	{
		stackError("dlsym(\"%s\") failed", arg_function);
		stackPushInt(0);
		return;
	}
	Com_DPrintf("function-name=%s -> address=%.8x\n", arg_function, (unsigned int)func);
	func();
	dlclose(handle);
	stackPushInt(1);
}

void gsc_utils_ExecuteString()
{
	char *str;
	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_utils_ExecuteString() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	Cmd_ExecuteString(str);
	stackPushInt(1);
}

void gsc_utils_sendgameservercommand()
{
	int clientNum;
	char *message;
	if ( ! stackGetParams("is", &clientNum, &message))
	{
		stackError("gsc_utils_sendgameservercommand() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	SV_GameSendServerCommand(clientNum, 0, message);
	stackPushInt(1);
}

void gsc_utils_scandir()
{
	char *dirname;
	if ( ! stackGetParams("s", &dirname))
	{
		stackError("gsc_utils_scandir() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	DIR *dir;
	struct dirent *dir_ent;
	dir = opendir(dirname);
	if ( ! dir)
	{
		stackPushUndefined();
		return;
	}
	stackPushArray();
	while ( (dir_ent = readdir(dir)) != NULL)
	{
		stackPushString(dir_ent->d_name);
		stackPushArrayLast();
	}
	closedir(dir);
}

void gsc_utils_fopen()
{
	FILE *file;
	char *filename, *mode;
	if ( ! stackGetParams("ss", &filename, &mode))
	{
		stackError("gsc_utils_sendgameservercommand() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	file = fopen(filename, mode);

	if (!file)
	{
		stackError("gsc_utils_fopen() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt((int)file);
}

void gsc_utils_fread()
{
	FILE *file;
	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_fread() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fread() returned a error");
		stackPushUndefined();
		return;
	}

	char buffer[256];
	int ret = fread(buffer, 1, 255, file);
	if ( ! ret)
	{
		stackPushUndefined();
		return;
	}
	buffer[ret] = '\0';
	stackPushString(buffer);
}

void gsc_utils_fwrite()
{
	FILE *file;
	char *buffer;
	if ( ! stackGetParams("is", &file, &buffer))
	{
		stackError("gsc_utils_fwrite() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fwrite() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt(fwrite(buffer, 1, strlen(buffer), file));
}

void gsc_utils_fclose()
{
	FILE *file;
	if ( ! stackGetParams("i", &file))
	{
		stackError("gsc_utils_fclose() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (!file)
	{
		stackError("gsc_utils_fclose() returned a error");
		stackPushUndefined();
		return;
	}

	stackPushInt( fclose(file) );
}

// http://code.metager.de/source/xref/RavenSoftware/jediacademy/code/game/g_utils.cpp#36
void gsc_G_FindConfigstringIndexOriginal()
{
	char *name;
	int min, max, create;
	if ( ! stackGetParams("siii", &name, &min, &max, &create))
	{
		stackError("gsc_G_FindConfigstringIndexOriginal() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	signed int (*sig)(char *name, int min, int max, int create, char *errormessage);
#if COD_VERSION == COD2_1_0
	*(int*)&sig = 0x0811AE70;
#elif COD_VERSION == COD2_1_2
	*(int*)&sig = 0x0811D1A4;
#elif COD_VERSION == COD2_1_3
	*(int*)&sig = 0x0811D300;
#endif
	int ret = sig(name, min, max, create, "G_FindConfigstringIndex() from GSC");
	ret += min; // the real array index
	stackPushInt(ret);
}

// simple version, without crash
void gsc_G_FindConfigstringIndex()
{
	char *name;
	int min, max;
	char* (*func)(int i);
	if ( ! stackGetParams("sii", &name, &min, &max))
	{
		stackError("gsc_G_FindConfigstringIndex() one or more arguments is undefined or has a wrong type");
		return;
	}
#if COD_VERSION == COD2_1_0
	*(int*)&func = 0x08091108;
#elif COD_VERSION == COD2_1_2
	*(int*)&func = 0x08092918;
#elif COD_VERSION == COD2_1_3
	*(int*)&func = 0x08092a1c;
#endif
	for (int i = 1; i < max; i++)
	{
		char *curitem = func(min + i);
		if ( ! *curitem)
			break;
		if ( ! strcasecmp(name, curitem))
		{
			stackPushInt(i + min);
			return;
		}
	}
	stackPushInt(0);
	return;
}

void gsc_get_configstring()
{
	int index;
	char* (*func)(int index);
	if ( ! stackGetParams("i", &index))
	{
		stackError("gsc_get_configstring() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
#if COD_VERSION == COD2_1_0
	*(int*)&func = 0x08091108;
#elif COD_VERSION == COD2_1_2
	*(int*)&func = 0x08092918;
#elif COD_VERSION == COD2_1_3
	*(int*)&func = 0x08092a1c;
#endif
	char *string = func(index);
	if ( ! *string )
		stackPushUndefined();
	else
		stackPushString(string);
}

void gsc_set_configstring()
{
	int index;
	char *string;
	int (*func)(int index, char *string);
	if ( ! stackGetParams("is", &index, &string))
	{
		stackError("gsc_set_configstring() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
#if COD_VERSION == COD2_1_0
	*(int*)&func = 0x08090E6C;
#elif COD_VERSION == COD2_1_2
	*(int*)&func = 0x0809267C;
#elif COD_VERSION == COD2_1_3
	*(int*)&func = 0x08092780;
#endif
	stackPushInt(func(index, string));
}

void gsc_call_function_raw()
{
	int func_address;
	char *args;
	unsigned char *data;
	if ( ! stackGetParams("isi", &func_address, &args, &data))
	{
		stackError("gsc_call_function_raw() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushInt(cracking_call_function(func_address, args, data));
}

void gsc_dlopen()
{
	char *arg_library;
	if ( ! stackGetParams("s", &arg_library))
	{
		stackError("gsc_dlopen() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int handle = (int)dlopen(arg_library, RTLD_LAZY);
	if ( ! handle)
	{
		stackError("dlopen(\"%s\") failed! Error: %s", arg_library, dlerror());
		stackPushInt(0);
		return;
	}
	stackPushInt(handle);
}

void gsc_dlsym()
{
	int handle;
	char *arg_function;
	if ( ! stackGetParams("is", &handle, &arg_function))
	{
		stackError("gsc_dlsym() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int func = (int)dlsym((void *)handle, arg_function);
	if (!func)
	{
		stackError("dlsym(\"%s\") failed! Error: %s", arg_function, dlerror());
		stackPushInt(0);
		return;
	}
	stackPushInt(func);
}

void gsc_dlclose()
{
	int handle;
	if ( ! stackGetParams("i", &handle))
	{
		stackError("gsc_dlclose() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	int ret = dlclose((void *)handle);
	if (ret != 0)
	{
		stackError("dlclose(%d) failed! Error: %s", handle, dlerror());
		stackPushInt(0);
		return;
	}
	stackPushInt(ret);
}

#define MAX_WEAPON_IGNORE_SIZE 20
#define MAX_WEAPON_NAME_SIZE 32
char* defaultweapon_mp = NULL;
char ignoredWeapons[MAX_WEAPON_IGNORE_SIZE][MAX_WEAPON_NAME_SIZE];
int ignoredWeaponCount = 0;

void gsc_utils_init()
{
	if(defaultweapon_mp == NULL)
		defaultweapon_mp = (char*)malloc(MAX_WEAPON_NAME_SIZE);
	if(defaultweapon_mp == NULL)
		stackError("gsc_utils_init() failed to malloc defaultweapon_mp");

	strcpy(defaultweapon_mp, "defaultweapon_mp");
	defaultweapon_mp[strlen(defaultweapon_mp)] = '\0';
}

void gsc_utils_free()
{
	free(defaultweapon_mp);
}

bool isOnIgnoreList(char* weapon)
{
	if(ignoredWeaponCount == 0)
		return false;

	for(int i=0; i<ignoredWeaponCount; i++)
	{
		if(strcmp(ignoredWeapons[i], weapon) == 0)
			return true;
	}

	return false;
}

int hook_findWeaponIndex(char* weapon)
{
	typedef int (*findIndexWeapon_t)(char* weapon);

#if COD_VERSION == COD2_1_0
	findIndexWeapon_t findIndexWeapon = (findIndexWeapon_t)0x080E949C;
#elif COD_VERSION == COD2_1_2
	findIndexWeapon_t findIndexWeapon = (findIndexWeapon_t)0x080EBA8C;
#elif COD_VERSION == COD2_1_3
	findIndexWeapon_t findIndexWeapon = (findIndexWeapon_t)0x080EBBD0;
#endif

	if(isOnIgnoreList(weapon))
		return findIndexWeapon(defaultweapon_mp);
	else
		return findIndexWeapon(weapon);
}

void gsc_utils_resetignoredweapons()
{
	ignoredWeaponCount = 0;
}

void gsc_utils_ignoreweapon()
{
	char* weapon;
	if ( ! stackGetParams("s", &weapon))
	{
		stackError("gsc_utils_ignoreweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if(strlen(weapon) > MAX_WEAPON_NAME_SIZE - 1)
	{
		stackError("gsc_utils_ignoreweapon() weapon name is too long");
		stackPushUndefined();
		return;
	}

	if(ignoredWeaponCount >= MAX_WEAPON_IGNORE_SIZE)
	{
		stackError("Exceeded MAX_WEAPON_IGNORE_SIZE %d", MAX_WEAPON_IGNORE_SIZE);
		stackPushUndefined();
		return;
	}

	strcpy(ignoredWeapons[ignoredWeaponCount], weapon);
	ignoredWeapons[ignoredWeaponCount][strlen(weapon)] = '\0';
	ignoredWeaponCount++;
	stackPushInt(1);
}

void gsc_utils_setdefaultweapon()
{
	char* weapon;
	if ( ! stackGetParams("s", &weapon))
	{
		stackError("gsc_utils_setdefaultweapon() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if(strlen(weapon) > MAX_WEAPON_NAME_SIZE - 1)
	{
		stackError("gsc_utils_setdefaultweapon() weapon name is too long");
		stackPushUndefined();
		return;
	}

	if(strcmp(defaultweapon_mp, weapon) == 0)
	{
		stackPushInt(2);
		return;
	}

	strcpy(defaultweapon_mp, weapon);
	defaultweapon_mp[strlen(weapon)] = '\0';
#if COD_VERSION == COD2_1_0
	memcpy((void*)0x0811E929, &defaultweapon_mp, 4); // default
	memcpy((void*)0x080E8AAD, &defaultweapon_mp, 4); // not found
	//memcpy((void*)0x080F014D, &defaultweapon_mp, 4); // not found backup
	memcpy((void*)0x080E928A, &defaultweapon_mp, 4); // unknown
#elif COD_VERSION == COD2_1_2
	memcpy((void*)0x08120C5A, &defaultweapon_mp, 4); // default
	memcpy((void*)0x080EB09D, &defaultweapon_mp, 4); // not found
	//memcpy((void*)0x080F273D, &defaultweapon_mp, 4); // not found backup
	memcpy((void*)0x080EB87A, &defaultweapon_mp, 4); // unknown
#elif COD_VERSION == COD2_1_3
	memcpy((void*)0x08120DB9, &defaultweapon_mp, 4); // default
	memcpy((void*)0x080EB1E1, &defaultweapon_mp, 4); // not found
	//memcpy((void*)0x080F2881, &defaultweapon_mp, 4); // not found backup
	memcpy((void*)0x080EB9BE, &defaultweapon_mp, 4); // unknown
#endif
	stackPushInt(1);
}

int weaponCount()
{
#if COD_VERSION == COD2_1_0
	return *(int*)0x08576140;
#elif COD_VERSION == COD2_1_2
	return *(int*)0x0858A000;
#elif COD_VERSION == COD2_1_3
	return *(int*)0x08627080; // see 80EBFFE (cod2 1.3)
#endif
}

int getWeapon(int index)
{
	typedef int (*get_weapon_t)(int index);

#if COD_VERSION == COD2_1_0
	get_weapon_t get_weapon = (get_weapon_t)0x080E9270;
#elif COD_VERSION == COD2_1_2
	get_weapon_t get_weapon = (get_weapon_t)0x080EB860;
#elif COD_VERSION == COD2_1_3
	get_weapon_t get_weapon = (get_weapon_t)0x080EB9A4;
#endif

	return get_weapon(index);
}

bool isValidWeaponId(int id)
{
	int weps = weaponCount();
	if(id >= weps || id < 0 || weps == 0)
		return false;

	return true;
}

void gsc_utils_getweaponoffsetint(char* funcname, int offset)
{
	int id;
	if ( ! stackGetParams("i", &id))
	{
		stackError("wrongs args for: %s(id)", funcname);
		stackPushInt(0);
		return;
	}

	if(!isValidWeaponId(id))
	{
		stackError("index is out of bounds: %s(id)", funcname);
		stackPushInt(0);
		return;
	}

	int value = *(int*)(getWeapon(id) + offset);
	stackPushInt(value);
}

void gsc_utils_setweaponoffsetint(char* funcname, int offset)
{
	int id;
	int value;
	if ( ! stackGetParams("ii", &id, &value))
	{
		stackError("scriptengine> wrongs args for: %s(id, value)", funcname);
		stackPushInt(0);
		return;
	}

	if(!isValidWeaponId(id))
	{
		stackError("index is out of bounds: %s(id, value)", funcname);
		stackPushInt(0);
		return;
	}

	int* index = (int*)(getWeapon(id) + offset);
	*index = value;
	stackPushInt(1);
}

void gsc_utils_getweaponmaxammo()
{
	gsc_utils_getweaponoffsetint("getweaponmaxammo", 468);
}

void gsc_utils_getweaponclipsize()
{
	gsc_utils_getweaponoffsetint("getweaponclipsize", 472);
}

void gsc_utils_getweapondamage()
{
	gsc_utils_getweaponoffsetint("getweapondamage", 492);
}

void gsc_utils_setweapondamage()
{
	gsc_utils_setweaponoffsetint("setweapondamage", 492);
}

void gsc_utils_getweaponmeleedamage()
{
	gsc_utils_getweaponoffsetint("getweapondamagemelee", 500);
}

void gsc_utils_setweaponmeleedamage()
{
	gsc_utils_setweaponoffsetint("setweapondamagemelee", 500);
}

void gsc_utils_getweaponfiretime()
{
	gsc_utils_getweaponoffsetint("getweaponfiretime", 516);
}

void gsc_utils_setweaponfiretime()
{
	gsc_utils_setweaponoffsetint("setweaponfiretime", 516); // see 80EF58A
}

void gsc_utils_getweaponmeleetime()
{
	gsc_utils_getweaponoffsetint("getweaponmeleetime", 532);
}

void gsc_utils_setweaponmeleetime()
{
	gsc_utils_setweaponoffsetint("setweaponmeleetime", 532);
}

void gsc_utils_getweaponreloadtime()
{
	gsc_utils_getweaponoffsetint("getweaponreloadtime", 536);
}

void gsc_utils_setweaponreloadtime()
{
	gsc_utils_setweaponoffsetint("setweaponreloadtime", 536);
}

void gsc_utils_getweaponreloademptytime()
{
	gsc_utils_getweaponoffsetint("getweaponreloademptytime", 540);
}

void gsc_utils_setweaponreloademptytime()
{
	gsc_utils_setweaponoffsetint("setweaponreloademptytime", 540);
}

char* hitlocs[] = {"none", "helmet", "head", "neck", "torso_upper", "torso_lower", "right_arm_upper",
                   "right_arm_lower", "right_hand", "left_arm_upper", "left_arm_lower", "left_hand", "right_leg_upper",
                   "right_leg_lower", "right_foot", "left_leg_upper", "left_leg_lower", "left_foot", "gun"
                  };

int getHitLocOffset(char* hitloc)
{
	int offset = 0; // none
	for (int i=0; i<19; i++) // prevent out of bound
	{
		if(strcmp(hitlocs[i], hitloc) == 0)
		{
			offset = i;
			break;
		}
	}
	return offset;
}

void gsc_utils_getweaponhitlocmultiplier()
{
	int id;
	char* hitloc;
	if ( ! stackGetParams("is", &id, &hitloc))
	{
		stackError("gsc_utils_getweaponhitlocmultiplier() one or more arguments is undefined or has a wrong type");
		stackPushInt(0);
		return;
	}

	if(!isValidWeaponId(id))
	{
		stackError("index is out of bounds: getweaponhitlocmultiplier(id, hitloc)");
		stackPushInt(0);
		return;
	}

	int offset = getHitLocOffset(hitloc);
	float multiplier = *(float*)(getWeapon(id) + 4 * offset + 1456);
	stackPushFloat(multiplier);
}

void gsc_utils_setweaponhitlocmultiplier()
{
	int id;
	float multiplier;
	char* hitloc;
	if ( ! stackGetParams("isf", &id, &hitloc, &multiplier))
	{
		stackError("gsc_utils_setweaponhitlocmultiplier() one or more arguments is undefined or has a wrong type");
		stackPushInt(0);
		return;
	}

	if(!isValidWeaponId(id))
	{
		stackError("index out of bounds: getweaponhitlocmultiplier(id, hitloc, multiplier)");
		stackPushInt(0);
		return;
	}

	int offset = getHitLocOffset(hitloc);
	float* multiPointer = (float*)(getWeapon(id) + 4 * offset + 1456);
	*multiPointer = multiplier;
	stackPushFloat(1);
}

void gsc_utils_getloadedweapons()
{
	stackPushArray();
	int weps = weaponCount();
	if(weps == 0)
		return;

	for(int i=0; i<weps; i++)
	{
		int w = getWeapon(i);
		stackPushString(*(char**)w);
		stackPushArrayLast();
	}

	// the offset are written in hex after each name (e.g fireTime at 8187084 with 0x204 (516))
	// 0 = weapon_mp
	// 4 = display name
	// 468 = max ammo
	// 472 = clip size
	// 476 = shot count
	// 492 = damage
	// 500 = melee damage
	// 612 = moveSpeedScale // see 80E1C58 (cod2 1.3) call 80E268A
	// 1456 - 1528 = locNone till locGun
	// [id][weapon_mp][worldmodel][viewmodel]: displayname
}

void gsc_utils_sqrt()
{
	float x;
	if ( ! stackGetParams("f", &x))
	{
		stackError("gsc_utils_sqrt() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	stackPushFloat(sqrt(x));
}

void gsc_utils_sqrtInv()
{
	float x;
	if ( ! stackGetParams("f", &x))
	{
		stackError("gsc_utils_sqrtInv() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	// http://www.beyond3d.com/content/articles/8/
	float xhalf = 0.5f*x;
	int i = *(int*)&x;
	i = 0x5f3759df - (i>>1);
	x = *(float*)&i;
	x = x*(1.5f - xhalf*x*x);

	stackPushFloat(x);
}

#endif
