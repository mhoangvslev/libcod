#include "gsc.hpp"

/*
	CoD2 search for "parameter count exceeds 256" and go upwards
*/

#if COD_VERSION == COD2_1_0
Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08115824;
Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x0811595C;
#elif COD_VERSION == COD2_1_2
Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117B56;
Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117C8E;
#elif COD_VERSION == COD2_1_3
Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117CB2;
Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117DEA;
#endif

#if COD_VERSION == COD2_1_0 // search "animation '%s' not defined in anim tree '%s'"
unsigned short (*GetVariableName)(unsigned short) = (unsigned short(*)(unsigned short))0x0807CA72;
unsigned short (*GetNextVariable)(unsigned short) = (unsigned short(*)(unsigned short))0x0807C9CE; //idk original funcname
#elif COD_VERSION == COD2_1_2
unsigned short (*GetVariableName)(unsigned short) = (unsigned short(*)(unsigned short))0x0807CFF6;
unsigned short (*GetNextVariable)(unsigned short) = (unsigned short(*)(unsigned short))0x0807CF52; //idk original funcname
#elif COD_VERSION == COD2_1_3
unsigned short (*GetVariableName)(unsigned short) = (unsigned short(*)(unsigned short))0x0807D0C2;
unsigned short (*GetNextVariable)(unsigned short) = (unsigned short(*)(unsigned short))0x0807D01E; //idk original funcname
#endif

#if COD_VERSION == COD2_1_0
char *(*SL_ConvertToString)(unsigned short) = (char*(*)(unsigned short))0x08078896;
#elif COD_VERSION == COD2_1_2
char *(*SL_ConvertToString)(unsigned short) = (char*(*)(unsigned short))0x08078E1A;
#elif COD_VERSION == COD2_1_3
char *(*SL_ConvertToString)(unsigned short) = (char*(*)(unsigned short))0x08078EE6;
#endif

char *stackGetParamTypeAsString(int param)
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;

	char *type;

	switch (arg->type)
	{
	case  0:
		type = "UNDEFINED";
		break;

	case  1:
		type = "OBJECT";
		break;

	case  2:
		type = "STRING";
		break;

	case  3:
		type = "LOCALIZED_STRING";
		break;

	case  4:
		type = "VECTOR";
		break;

	case  5:
		type = "FLOAT";
		break;

	case  6:
		type = "INT";
		break;

	case  7:
		type = "CODEPOS";
		break;

	case  8:
		type = "PRECODEPOS";
		break;

	case  9:
		type = "FUNCTION";
		break;

	case 10:
		type = "STACK";
		break;

	case 11:
		type = "ANIMATION";
		break;

	case 12:
		type = "DEVELOPER_CODEPOS";
		break;

	case 13:
		type = "INCLUDE_CODEPOS";
		break;

	case 14:
		type = "THREAD_LIST";
		break;

	case 15:
		type = "THREAD_1";
		break;

	case 16:
		type = "THREAD_2";
		break;

	case 17:
		type = "THREAD_3";
		break;

	case 18:
		type = "THREAD_4";
		break;

	case 19:
		type = "STRUCT";
		break;

	case 20:
		type = "REMOVED_ENTITY";
		break;

	case 21:
		type = "ENTITY";
		break;

	case 22:
		type = "ARRAY";
		break;

	case 23:
		type = "REMOVED_THREAD";
		break;

	default:
		type = "UNKNOWN TYPE";
		break;
	}

	return type;
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
		printf("scriptengine> WARNING: printf undefined argument!\n");
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
void gsc_utils_printfline()
{
	gsc_utils_printf();
	printf("\n");
}

void gsc_utils_com_printf()
{
	char *str;
	if ( ! stackGetParams("s", &str))
	{
		printf("scriptengine> WARNING: com_printf undefined argument!\n");
		stackPushUndefined();
		return;
	}

	Com_Printf("%s", str);
	stackPushInt(1);
}

void gsc_utils_redirectprintf()
{
	char *str;
	if ( ! stackGetParams("s", &str))
	{
		printf("scriptengine> WARNING: gsc_utils_redirectprintf undefined argument!\n");
		stackPushUndefined();
		return;
	}

	SV_FlushRedirect(str);
	stackPushInt(1);
}

Scr_Function scriptFunctions[] =
{
	{"printf", gsc_utils_printf, 0},
	{"printfline", gsc_utils_printfline, 0}, // adds \n at end
	{"com_printf", gsc_utils_com_printf, 0},
	{"redirectprintf", gsc_utils_redirectprintf, 0},
	{"getArrayKeys", Scr_GetArrayKeys, 0},

#if COMPILE_MYSQL == 1
	{"mysql_init"              , gsc_mysql_init              , 0},
	{"mysql_real_connect"      , gsc_mysql_real_connect      , 0},
	{"mysql_close"             , gsc_mysql_close             , 0},
	{"mysql_query"             , gsc_mysql_query             , 0},
	{"mysql_errno"             , gsc_mysql_errno             , 0},
	{"mysql_error"             , gsc_mysql_error             , 0},
	{"mysql_affected_rows"     , gsc_mysql_affected_rows     , 0},
	{"mysql_store_result"      , gsc_mysql_store_result      , 0},
	{"mysql_num_rows"          , gsc_mysql_num_rows          , 0},
	{"mysql_num_fields"        , gsc_mysql_num_fields        , 0},
	{"mysql_field_seek"        , gsc_mysql_field_seek        , 0},
	{"mysql_fetch_field"       , gsc_mysql_fetch_field       , 0},
	{"mysql_fetch_row"         , gsc_mysql_fetch_row         , 0},
	{"mysql_free_result"       , gsc_mysql_free_result       , 0},
	{"mysql_real_escape_string", gsc_mysql_real_escape_string, 0},
	{"mysql_async_create_query", gsc_mysql_async_create_query, 0},
	{"mysql_async_create_query_nosave", gsc_mysql_async_create_query_nosave, 0},
	{"mysql_async_getdone_list", gsc_mysql_async_getdone_list, 0},
	{"mysql_async_getresult_and_free", gsc_mysql_async_getresult_and_free, 0},
	{"mysql_async_initializer" , gsc_mysql_async_initializer , 0},
	{"mysql_reuse_connection"  , gsc_mysql_reuse_connection  , 0},
#endif

#if COMPILE_PLAYER == 1
	{"free_slot"                   , gsc_free_slot                         , 0},
	{"kick2"                       , gsc_kick_slot                         , 0},
	{"fpsnextframe"                , gsc_fpsnextframe                      , 0},
#endif

#if COMPILE_MEMORY == 1
	{"memory_malloc" , gsc_memory_malloc , 0},
	{"memory_free"   , gsc_memory_free   , 0},
	{"memory_int_get", gsc_memory_int_get, 0},
	{"memory_int_set", gsc_memory_int_set, 0},
	{"memory_memset" , gsc_memory_memset , 0},
	{"binarybuffer_new"  , gsc_binarybuffer_new  , 0},
	{"binarybuffer_free" , gsc_binarybuffer_free , 0},
	{"binarybuffer_seek" , gsc_binarybuffer_seek , 0},
	{"binarybuffer_write", gsc_binarybuffer_write, 0},
	{"binarybuffer_read" , gsc_binarybuffer_read , 0},
#endif

#if COMPILE_UTILS == 1
	{"disableGlobalPlayerCollision"   , gsc_utils_disableGlobalPlayerCollision, 0},
	{"disableGlobalPlayerEject"       , gsc_utils_disableGlobalPlayerEject    , 0},
	{"getAscii"                       , gsc_utils_getAscii                    , 0},
	{"toUpper"                        , gsc_utils_toupper                     , 0},
	{"system"                         , gsc_utils_system                      , 0},
	{"execute"                        , gsc_utils_execute                     , 0},
	{"exponent"                       , gsc_utils_exponent                    , 0},
	{"file_link"                      , gsc_utils_file_link                   , 0},
	{"file_unlink"                    , gsc_utils_file_unlink                 , 0},
	{"file_exists"                    , gsc_utils_file_exists                 , 0},
	{"FS_LoadDir"                     , gsc_utils_FS_LoadDir                  , 0},
	{"getType"                        , gsc_utils_getType                     , 0},
	{"stringToFloat"                  , gsc_utils_stringToFloat               , 0},
	{"rundll"                         , gsc_utils_rundll                      , 0},
	{"Cmd_ExecuteString"              , gsc_utils_ExecuteString               , 0},
	{"sendGameServerCommand"          , gsc_utils_sendgameservercommand       , 0},
	{"scandir"                        , gsc_utils_scandir                     , 0},
	{"fopen"                          , gsc_utils_fopen                       , 0},
	{"fread"                          , gsc_utils_fread                       , 0},
	{"fwrite"                         , gsc_utils_fwrite                      , 0},
	{"fclose"                         , gsc_utils_fclose                      , 0},
	{"sprintf"                        , gsc_utils_sprintf                     , 0},
	{"add_language"                   , gsc_add_language                      , 0},
	{"load_languages"                 , gsc_load_languages                    , 0},
	{"get_language_item"              , gsc_get_language_item                 , 0},
	{"themetext"                      , gsc_themetext                         , 0},
	{"G_FindConfigstringIndex"        , gsc_G_FindConfigstringIndex           , 0},
	{"G_FindConfigstringIndexOriginal", gsc_G_FindConfigstringIndexOriginal   , 0},
	{"getconfigstring"                , gsc_get_configstring                  , 0},
	{"setconfigstring"                , gsc_set_configstring                  , 0},
	{"call_function_raw"              , gsc_call_function_raw                 , 0},
	{"dlopen"                         , gsc_dlopen                            , 0},
	{"dlsym"                          , gsc_dlsym                             , 0},
	{"dlclose"                        , gsc_dlclose                           , 0},
	{"resetignoredweapons"            , gsc_utils_resetignoredweapons         , 0},
	{"ignoreweapon"                   , gsc_utils_ignoreweapon                , 0},
	{"setdefaultweapon"               , gsc_utils_setdefaultweapon            , 0},
	{"getweaponmaxammo"               , gsc_utils_getweaponmaxammo            , 0},
	{"getweaponclipsize"              , gsc_utils_getweaponclipsize           , 0},
	{"getweapondamage"                , gsc_utils_getweapondamage             , 0},
	{"setweapondamage"                , gsc_utils_setweapondamage             , 0},
	{"getweaponmeleedamage"           , gsc_utils_getweaponmeleedamage        , 0},
	{"setweaponmeleedamage"           , gsc_utils_setweaponmeleedamage        , 0},
	{"getweaponfiretime"              , gsc_utils_getweaponfiretime           , 0},
	{"setweaponfiretime"              , gsc_utils_setweaponfiretime           , 0},
	{"getweaponmeleetime"             , gsc_utils_getweaponmeleetime          , 0},
	{"setweaponmeleetime"             , gsc_utils_setweaponmeleetime          , 0},
	{"getweaponreloadtime"            , gsc_utils_getweaponreloadtime         , 0},
	{"setweaponreloadtime"            , gsc_utils_setweaponreloadtime         , 0},
	{"getweaponreloademptytime"       , gsc_utils_getweaponreloademptytime    , 0},
	{"setweaponreloademptytime"       , gsc_utils_setweaponreloademptytime    , 0},
	{"getweaponhitlocmultiplier"      , gsc_utils_getweaponhitlocmultiplier   , 0},
	{"setweaponhitlocmultiplier"      , gsc_utils_setweaponhitlocmultiplier   , 0},
	{"getloadedweapons"               , gsc_utils_getloadedweapons            , 0},
	{"sqrt"                           , gsc_utils_sqrt                        , 0},
	{"sqrtInv"                        , gsc_utils_sqrtInv                     , 0},
#endif

	{NULL, NULL, 0} /* terminator */
};

Scr_FunctionCall Scr_GetCustomFunction(const char **fname, int *fdev)
{
	//printf("Scr_GetCustomFunction: fdev=%d fname=%s\n", *fdev, *fname);
	Scr_FunctionCall m = Scr_GetFunction(fname, fdev);
	if (m)
		return m;

	for (int i = 0; scriptFunctions[i].name; i++)
	{
		if (strcasecmp(*fname, scriptFunctions[i].name))
			continue;

		Scr_Function func = scriptFunctions[i];
		*fname = func.name;
		*fdev = func.developer;
		return func.call;
	}

	return NULL;
}

void gsc_player_printf(int id)
{
	printf("id: %.8x\n", id);
}

Scr_Method scriptMethods[] =
{
	{"printf", gsc_player_printf, 0}, // rather use sprintf() to re-use iprintlnbold() etc.?

#if COMPILE_PLAYER == 1
	{"getStance"             , gsc_player_stance_get         , 0},
	{"setVelocity"           , gsc_player_velocity_set       , 0},
	{"addVelocity"           , gsc_player_velocity_add       , 0},
	{"getVelocity"           , gsc_player_velocity_get       , 0},
	{"aimButtonPressed"      , gsc_player_button_ads         , 0},
	{"leftButtonPressed"     , gsc_player_button_left        , 0},
	{"rightButtonPressed"    , gsc_player_button_right       , 0},
	{"forwardButtonPressed"  , gsc_player_button_forward     , 0},
	{"backButtonPressed"     , gsc_player_button_back        , 0},
	{"leanleftButtonPressed" , gsc_player_button_leanleft    , 0},
	{"leanrightButtonPressed", gsc_player_button_leanright   , 0},
	{"jumpButtonPressed"     , gsc_player_button_jump        , 0},
	{"reloadButtonPressed"   , gsc_player_button_reload      , 0},
	{"getIP"                 , gsc_player_getip              , 0},
	{"getPing"               , gsc_player_getping            , 0},
	{"getSpectatorClient"    , gsc_player_spectatorclient_get, 0},
	{"ClientCommand"         , gsc_player_ClientCommand      , 0},
	{"getLastConnectTime"    , gsc_player_getLastConnectTime , 0},
	{"getLastMSG"            , gsc_player_getLastMSG         , 0},
	{"getAddressType"        , gsc_player_addresstype        , 0},
	{"getClientState"        , gsc_player_getclientstate     , 0},
	{"renameClient"          , gsc_player_renameclient       , 0},
	{"setAlive"              , gsc_entity_setalive           , 0},
	{"setBounds"             , gsc_entity_setbounds          , 0},
	{"get_userinfo"          , gsc_get_userinfo              , 0},
	{"set_userinfo"          , gsc_set_userinfo              , 0},
	{"printOutOfBand"        , gsc_player_outofbandprint     , 0},
	{"connectionlessPacket"  , gsc_player_connectionlesspacket, 0},
	{"clientuserinfochanged" , gsc_player_clientuserinfochanged, 0},
	{"resetNextReliableTime" , gsc_player_resetNextReliableTime, 0},
	{"setg_speed"            , gsc_player_setg_speed         , 0},
	{"setg_gravity"          , gsc_player_setg_gravity       , 0},
	{"setweaponfiremeleedelay", gsc_player_setweaponfiremeleedelay, 0},
	{"disableitempickup"     , gsc_player_disable_item_pickup, 0},
	{"enableitempickup"      , gsc_player_enable_item_pickup , 0},
	{"setanim"               , gsc_player_set_anim           , 0},
#if COMPILE_BOTS == 1
	{"setwalkdir"            , gsc_player_set_walkdir        , 0},
	{"setlean"               , gsc_player_set_lean           , 0},
	{"setstance"             , gsc_player_set_stance         , 0},
	{"thrownade"             , gsc_player_thrownade          , 0},
	{"fireweapon"            , gsc_player_fireweapon         , 0},
	{"meleeweapon"           , gsc_player_meleeweapon        , 0},
	{"reloadweapon"          , gsc_player_reloadweapon       , 0},
	{"adsaim"                , gsc_player_adsaim             , 0},
#endif
	{"getcooktime"           , gsc_player_getcooktime        , 0},
	{"setguid"               , gsc_player_setguid            , 0},
	{"clienthasclientmuted"  , gsc_player_clienthasclientmuted , 0},
	{"getlastgamestatesize"  , gsc_player_getlastgamestatesize , 0},
	{"resetfps"              , gsc_player_resetfps           , 0},
	{"getfps"                , gsc_player_getfps             , 0},
	{"setmovespeedscale"     , gsc_player_setmovespeedscale  , 0},
	{"ismantling"            , gsc_player_ismantling         , 0},
	{"isonladder"            , gsc_player_isonladder         , 0},
#endif

	{NULL, NULL, 0} /* terminator */
};

Scr_MethodCall Scr_GetCustomMethod(const char **fname, int *fdev)
{
	//printf("Scr_GetCustomMethod: fdev=%d fname=%s\n", *fdev, *fname);
	Scr_MethodCall m = Scr_GetMethod(fname, fdev);
	if (m)
		return m;

	for (int i = 0; scriptMethods[i].name; i++)
	{
		if (strcasecmp(*fname, scriptMethods[i].name))
			continue;

		Scr_Method func = scriptMethods[i];
		*fname = func.name;
		*fdev = func.developer;
		return func.call;
	}

	return NULL;
}

/*
	In CoD2 this address can be found in every get-param-function
		the stack-address is in a context like: dword_830AE88 - 8 * a1
*/
int getStack()
{
#if COD_VERSION == COD2_1_0
	return 0x083D7610;
#elif COD_VERSION == COD2_1_2
	return 0x083D7A10; // diff to 1.3: 1080
#elif COD_VERSION == COD2_1_3
	return 0x083D8A90;
#endif
}

int stackGetParamType(int param)
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;
	return arg->type;
}

int stackGetParams(char *params, ...)
{
	va_list args;
	va_start(args, params);

	int errors = 0;

	int len = strlen(params);
	int i;
	for (i = 0; i < len; i++)
	{
		switch (params[i])
		{
		case ' ': // ignore param
			break;

		case 'i':
		{
			int *tmp = va_arg(args, int *);
			if ( ! stackGetParamInt(i, tmp))
			{
				printf("Param %d needs to be an int, %s=%d given! NumParams=%d\n", i, ">make function for this<", stackGetParamType(i), stackGetNumberOfParams());
				errors++;
			}
			break;
		}

		case 'v':
		{
			float *tmp = va_arg(args, float *);
			if ( ! stackGetParamVector(i, tmp))
			{
				printf("Param %d needs to be a vector, %s=%d given! NumParams=%d\n", i, ">make function for this<", stackGetParamType(i), stackGetNumberOfParams());
				errors++;
			}
			break;
		}

		case 'f':
		{
			float *tmp = va_arg(args, float *);
			if ( ! stackGetParamFloat(i, tmp))
			{
				printf("Param %d needs to be a float, %s=%d given! NumParams=%d\n", i, ">make function for this<", stackGetParamType(i), stackGetNumberOfParams());
				errors++;
			}
			break;
		}

		case 's':
		{
			char **tmp = va_arg(args, char **);
			if ( ! stackGetParamString(i, tmp))
			{
				printf("Param %d needs to be a string, %s=%d given! NumParams=%d\n", i, ">make function for this<", stackGetParamType(i), stackGetNumberOfParams());
				errors++;
			}
			break;
		}

		default:
			errors++;
			printf("[WARNING] stackGetParams: errors=%d Identifier '%c' is not implemented!\n", errors, params[i]);
			break;
		}
	}

	va_end(args);
	return errors == 0; // success if no errors
}

// function can be found in same context as getStack()
int getNumberOfParams() // as in stackNew()
{
#if COD_VERSION == COD2_1_0
	return 0x083D761C;
#elif COD_VERSION == COD2_1_2
	return 0x083D7A1C; // diff to 1.3: 1080
#elif COD_VERSION == COD2_1_3
	return 0x083D8A9C;
#endif
}

// todo: check if the parameter really exists (number_of_params)
int stackGetParamInt(int param, int *value)
{
	//printf("stackGetParamInt() start...");
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;

	if (arg->type != STACK_INT)
		return 0;

	*value = (int)arg->offsetData;
	//printf("... end\n");
	return 1;
}

int stackGetParamString(int param, char **value) // as in the sub-functions in getentarray (hard one, use the graph to find it)
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;

	if (arg->type != STACK_STRING)
		return 0;

#if COD_VERSION == COD2_1_0
	*value = (char *)(*(int *)0x08283E80 + 8*(int)arg->offsetData + 4);
#elif COD_VERSION == COD2_1_2
	*value = (char *)(*(int *)0x08205E80 + 8*(int)arg->offsetData + 4);
#elif COD_VERSION == COD2_1_3
	*value = (char *)(*(int *)0x08206F00 + 8*(int)arg->offsetData + 4);
#endif

	return 1;
}

int stackGetParamVector(int param, float value[3])
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;

	if (arg->type != STACK_VECTOR)
		return 0;

	value[0] = *(float *)((int)(arg->offsetData) + 0);
	value[1] = *(float *)((int)(arg->offsetData) + 4);
	value[2] = *(float *)((int)(arg->offsetData) + 8);

	return 1;
}

int stackGetParamFloat(int param, float *value)
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;

	if (arg->type == STACK_INT)
	{
		int asInteger;
		int ret = stackGetParamInt(param, &asInteger);

		if (!ret)
			return 0;

		*value = (float) asInteger;

		return 1;
	}

	// *value = (float)arg->offsetData;
	// gcc: error: pointer value used where a floating point value was expected
	// so i use the tmp for casting
	float tmp;

	if (arg->type != STACK_FLOAT)
		return 0;

	//swapEndian(&arg->offsetData); // DOESEN WORK EVEN WITH SWAP
	//*value = (float)(int)arg->offsetData; // DOESNT WORK
	// jeah gcc, you fucked me off!
	memcpy(&tmp, &arg->offsetData, 4); // cast to float xD
	*value = tmp;

	return 1;
}

int stackGetNumberOfParams()
{
	int numberOfParams = *(int *)getNumberOfParams();
	return numberOfParams;
}

//thanks to riicchhaarrd/php
unsigned short Scr_GetArray(int index)
{
	if(index >= stackGetNumberOfParams())
	{
		printf("scriptengine> Scr_GetArray: one parameter is required\n");
		return 0;
	}

	int stack = getStack();
	int base = *(int*)(stack - 8 * index);
	int vartype = *(int*)(base + 4);

	if(vartype == STACK_OBJECT) //VT_OBJECT
		return *(unsigned short*)base;

	printf("scriptengine> Scr_GetArray: the parameter must be an array\n");
	return 0;
}

void Scr_GetArrayKeys()
{
	unsigned short arrIndex = Scr_GetArray(0);
	stackPushArray();

	if(arrIndex == 0)
		return; // we didn't find a valid array

	unsigned short i;
	for(i = GetNextVariable(arrIndex); i != 0;)
	{
		stackPushString(SL_ConvertToString(GetVariableName(i)));
		stackPushArrayLast();

		i = GetNextVariable(i);
	}
}

/* THE BEGINNING of generalizing the push-value-functions! */
// pushing a new stack-element on stack
// available through getStack()
// 11.03.2013, Sido|Meine Jordans Instrumental, Generalisation is unstable!

/* search for "Internal script stack overflow", thats stackNew() */
/* can also be found in the next stackPush-functions */
int stackNew()
{
	int (*signature)();

#if COD_VERSION == COD2_1_0
	*((int *)(&signature)) = 0x080837B0;
#elif COD_VERSION == COD2_1_2
	*((int *)(&signature)) = 0x08083D2C;
#elif COD_VERSION == COD2_1_3
	*((int *)(&signature)) = 0x08083DF8;
#endif

	return signature();
}

int stackPushUndefined()
{
	aStackElement *scriptStack;

	scriptStack = *(aStackElement**)getStack();
#if DEBUG_GSC
	printf("stackPushUndefined(): type=%d value=%.8x\n", scriptStack->type, scriptStack->offsetData);
#endif
	int ret = stackNew();

	scriptStack = *(aStackElement**)getStack();
#if DEBUG_GSC
	printf("stackPushUndefined(): type=%d value=%.8x\n", scriptStack->type, scriptStack->offsetData);
#endif

	//aStackElement *scriptStack = *(aStackElement**)getStack();
	//aStackElement *scriptStack = *(aStackElement**)ret;
	scriptStack->type = STACK_UNDEFINED;
	scriptStack->offsetData = NULL; // never tested anything else for UNDEFINED
	//return (int) *(aStackElement**) getStack(); // dunno...
	return 1; // dunno... works also lol. so no need to return some specific stackelement
}

/*

	HOWTO: how to find the addresses of a binary?
	1) find the function-string in winhex
	2) go to file offset in IDA -> then copy the real address
	3) then search for the SWAPPED value in winhex again
	4) go for it in ida
	5) convert the crazy numbers to str/func-pair
	6) go into function and get the offset of the internal function
*/

int stackPushInt(int ret) // as in isalive
{
	int (*signature)(int);

#if COD_VERSION == COD2_1_0
	*((int *)(&signature)) = 0x08084B1C;
#elif COD_VERSION == COD2_1_2
	*((int *)(&signature)) = 0x08085098; // difference to 1.3: CC
#elif COD_VERSION == COD2_1_3
	*((int *)(&signature)) = 0x08085164;
#endif

	return signature(ret);
}

int stackPushVector(float *ret) // as in vectornormalize
{
	int (*signature)(float *);

#if COD_VERSION == COD2_1_0
	*((int *)(&signature)) = 0x08084CBE;
#elif COD_VERSION == COD2_1_2
	*((int *)(&signature)) = 0x0808523A; // difference to 1.3: CC
#elif COD_VERSION == COD2_1_3
	*((int *)(&signature)) = 0x08085306;
#endif

	return signature(ret);
}

int stackPushFloat(float ret) // as in distance
{
	int (*signature)(float);

#if COD_VERSION == COD2_1_0
	*((int *)(&signature)) = 0x08084B40;
#elif COD_VERSION == COD2_1_2
	*((int *)(&signature)) = 0x080850BC; // difference to 1.3: CC
#elif COD_VERSION == COD2_1_3
	*((int *)(&signature)) = 0x08085188;
#endif

	return signature(ret);
}

int stackPushString(char *toPush) // as in getcvar()
{
	int (*signature)(char *);

#if COD_VERSION == COD2_1_0
	*((int *)(&signature)) = 0x08084C1A;
#elif COD_VERSION == COD2_1_2
	*((int *)(&signature)) = 0x08085196; // difference to 1.3: CC
#elif COD_VERSION == COD2_1_3
	*((int *)(&signature)) = 0x08085262;
#endif

	return signature(toPush);
}

int stackPushEntity(int arg) // as in getent() // todo: find out how to represent an entity
{
	int (*signature)(int);

#if COD_VERSION == COD2_1_0
	*((int *)(&signature)) = 0x08118CC0;
#elif COD_VERSION == COD2_1_2
	*((int *)(&signature)) = 0x0811AFF4; // difference OTHER then CC
#elif COD_VERSION == COD2_1_3
	*((int *)(&signature)) = 0x08117F50;
#endif

	return signature(arg);
}

// as in bullettrace
int stackPushArray()
{
	int (*signature)();

#if COD_VERSION == COD2_1_0
	*((int *)(&signature)) = 0x08084CF0;
#elif COD_VERSION == COD2_1_2
	*((int *)(&signature)) = 0x0808526C;
#elif COD_VERSION == COD2_1_3
	*((int *)(&signature)) = 0x08085338;
#endif

	return signature();
}

int stackPushArrayLast()   // as in getentarray
{
	int (*signature)();

#if COD_VERSION == COD2_1_0
	*((int *)(&signature)) = 0x08084D1C;
#elif COD_VERSION == COD2_1_2
	*((int *)(&signature)) = 0x08085298;
#elif COD_VERSION == COD2_1_3
	*((int *)(&signature)) = 0x08085364;
#endif

	return signature();
}