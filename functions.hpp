#if !defined(_FUNCTIONS_HPP_) && defined(COD_VERSION)
#define _FUNCTIONS_HPP_

/* MAKE FUNCTIONS STATIC, SO THEY CAN BE IN EVERY FILE */

typedef unsigned short (*GetVariableName_t)(unsigned short a1);
#if COD_VERSION == COD2_1_0
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CA72;
#elif COD_VERSION == COD2_1_2
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807CFF6;
#elif COD_VERSION == COD2_1_3
static const GetVariableName_t GetVariableName = (GetVariableName_t)0x0807D0C2;
#endif

typedef unsigned short (*GetNextVariable_t)(unsigned short a1);
#if COD_VERSION == COD2_1_0
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807C9CE;
#elif COD_VERSION == COD2_1_2
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807CF52;
#elif COD_VERSION == COD2_1_3
static const GetNextVariable_t GetNextVariable = (GetNextVariable_t)0x0807D01E;
#endif

typedef char * (*SL_ConvertToString_t)(int a1);
#if COD_VERSION == COD2_1_0
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078896;
#elif COD_VERSION == COD2_1_2
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078E1A;
#elif COD_VERSION == COD2_1_3
static const SL_ConvertToString_t SL_ConvertToString = (SL_ConvertToString_t)0x08078EE6;
#endif

typedef int (*codscript_load_function_t)(char *file, char *function, int isNeeded);
#if COD_VERSION == COD2_1_0
static const codscript_load_function_t codscript_load_function = (codscript_load_function_t)0x0810DD70;
#elif COD_VERSION == COD2_1_2
static const codscript_load_function_t codscript_load_function = (codscript_load_function_t)0x081100AC;
#elif COD_VERSION == COD2_1_3
static const codscript_load_function_t codscript_load_function = (codscript_load_function_t)0x08110208;
#endif

typedef char * (*Cmd_Argv_t)(int arg);
#if COD_VERSION == COD2_1_0
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x0806001C;
#elif COD_VERSION == COD2_1_2
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060228;
#elif COD_VERSION == COD2_1_3
static const Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060220;
#endif

typedef int (*trap_Argv_t)(unsigned int param, char *buf, int bufLen);
#if COD_VERSION == COD2_1_0
static const trap_Argv_t trap_Argv = (trap_Argv_t)0x08060074;
#elif COD_VERSION == COD2_1_2
static const trap_Argv_t trap_Argv = (trap_Argv_t)0x08060280;
#elif COD_VERSION == COD2_1_3
static const trap_Argv_t trap_Argv = (trap_Argv_t)0x08060278;
#endif

typedef int (*Com_Printf_t)(const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060B2C;
#elif COD_VERSION == COD2_1_2
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060DF2;
#elif COD_VERSION == COD2_1_3
static const Com_Printf_t Com_Printf = (Com_Printf_t)0x08060DEA;
#endif

typedef int (*Com_DPrintf_t)(const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060B7C;
#elif COD_VERSION == COD2_1_2
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060E42;
#elif COD_VERSION == COD2_1_3
static const Com_DPrintf_t Com_DPrintf = (Com_DPrintf_t)0x08060E3A;
#endif

typedef int (*Com_sprintf_t)(char *dest, int size, const char *format, ...);
#if COD_VERSION == COD2_1_0
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B5932;
#elif COD_VERSION == COD2_1_2
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B7DC6;
#elif COD_VERSION == COD2_1_3
static const Com_sprintf_t Com_sprintf = (Com_sprintf_t)0x080B7F0A;
#endif

typedef int (*Cmd_ExecuteString_t)(const char *text);
#if COD_VERSION == COD2_1_0
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x08060754;
#elif COD_VERSION == COD2_1_2
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609D4;
#elif COD_VERSION == COD2_1_3
static const Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609CC;
#endif

typedef int (*ClientCommand_t)(int clientNum);
#if COD_VERSION == COD2_1_0
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x080FE998; // search 'say_team' and see code xref function
#elif COD_VERSION == COD2_1_2
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x08100D1E;
#elif COD_VERSION == COD2_1_3
static const ClientCommand_t ClientCommand = (ClientCommand_t)0x08100E62;
#endif

typedef int (*Cvar_VariableValue_t)(const char *var_name); // search for 'sv_allowAnonymous'
#if COD_VERSION == COD2_1_0
static const Cvar_VariableValue_t CvarVariableValue = (Cvar_VariableValue_t)0x080B0BB6;
#elif COD_VERSION == COD2_1_2
static const Cvar_VariableValue_t CvarVariableValue = (Cvar_VariableValue_t)0x080B2E66;
#elif COD_VERSION == COD2_1_3
static const Cvar_VariableValue_t CvarVariableValue = (Cvar_VariableValue_t)0x080B2FAA;
#endif

typedef char * (*Cvar_VariableString_t)(const char *var_name);
#if COD_VERSION == COD2_1_0
static const Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)0x080B0D96;
#elif COD_VERSION == COD2_1_2
static const Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)0x080B3046;
#elif COD_VERSION == COD2_1_3
static const Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)0x080B318A;
#endif

typedef int (*FS_ReadFile_t)(const char *qpath, void **buffer);
#if COD_VERSION == COD2_1_0
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x0809E892;
#elif COD_VERSION == COD2_1_2
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0958;
#elif COD_VERSION == COD2_1_3
static const FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0A9C;
#endif

typedef int (*FS_LoadDir_t)(char *path, char *dir);
#if COD_VERSION == COD2_1_0
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A01A4;
#elif COD_VERSION == COD2_1_2
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A22D8;
#elif COD_VERSION == COD2_1_3
static const FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A241C;
#endif

typedef enum
{
	CS_FREE,		// can be reused for a new connection
	CS_ZOMBIE,		// client has been disconnected, but don't reuse connection for a couple seconds
	CS_CONNECTED,	// has been assigned to a client_t, but no gamestate yet
	CS_PRIMED,		// gamestate has been sent, but client hasn't sent a usercmd
	CS_ACTIVE		// client is fully in game
} clientState_t;

typedef enum
{
	NA_BOT,
	NA_BAD, // an address lookup failed
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
	NA_IPX,
	NA_BROADCAST_IPX
} netadrtype_t;

typedef struct
{
	netadrtype_t type;
	unsigned char ip[4];
	char ipx[10];
	unsigned short port;
} netadr_t;

typedef struct
{
	bool overflowed; // 0
	char *data; // 4
	int maxsize; // 8
	int cursize; // 12
	int readcount; // 16
	int bit;
} msg_t; // 0x18

typedef int (*SV_ConnectionlessPacket_t)(netadr_t from, msg_t * msg);
#if COD_VERSION == COD2_1_0
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x08093F1E;
#elif COD_VERSION == COD2_1_2
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x08095894;
#elif COD_VERSION == COD2_1_3
static const SV_ConnectionlessPacket_t SV_ConnectionlessPacket = (SV_ConnectionlessPacket_t)0x0809594E;
#endif

typedef int (*NET_OutOfBandPrint_t)( int sock, netadr_t adr, const char *msg );
#if COD_VERSION == COD2_1_0
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C40C;
#elif COD_VERSION == COD2_1_2
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C8D4;
#elif COD_VERSION == COD2_1_3
static const NET_OutOfBandPrint_t NET_OutOfBandPrint = (NET_OutOfBandPrint_t)0x0806C8CC;
#endif

typedef int (*SV_GameSendServerCommand_t)(int clientNum, signed int a2, const char *msg);
#if COD_VERSION == COD2_1_0
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x0808FE96;
#elif COD_VERSION == COD2_1_2
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080916A6;
#elif COD_VERSION == COD2_1_3
static const SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080917AA;
#endif

typedef int (*SV_DropClient_t)(int a1, char* message);
#if COD_VERSION == COD2_1_0
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808DC8C;
#elif COD_VERSION == COD2_1_2
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808EF9A;
#elif COD_VERSION == COD2_1_3
static const SV_DropClient_t SV_DropClient = (SV_DropClient_t)0x0808F02E;
#endif

typedef int (*SV_WriteDownloadToClient_t)(int a1, int a2);
#if COD_VERSION == COD2_1_0
static const SV_WriteDownloadToClient_t SV_WriteDownloadToClient = (SV_WriteDownloadToClient_t)0x0808E544;
#elif COD_VERSION == COD2_1_2
static const SV_WriteDownloadToClient_t SV_WriteDownloadToClient = (SV_WriteDownloadToClient_t)0x0808FD2E;
#elif COD_VERSION == COD2_1_3
static const SV_WriteDownloadToClient_t SV_WriteDownloadToClient = (SV_WriteDownloadToClient_t)0x0808FDC2;
#endif

typedef int (*SV_BeginDownload_f_t)(int a1);
#if COD_VERSION == COD2_1_0
static const SV_BeginDownload_f_t SV_BeginDownload_f = (SV_BeginDownload_f_t)0x0808E508;
#elif COD_VERSION == COD2_1_2
static const SV_BeginDownload_f_t SV_BeginDownload_f = (SV_BeginDownload_f_t)0x0808F8AC;
#elif COD_VERSION == COD2_1_3
static const SV_BeginDownload_f_t SV_BeginDownload_f = (SV_BeginDownload_f_t)0x0808F940;
#endif

typedef int (*ClientUserinfoChanged_t)(int a1);
#if COD_VERSION == COD2_1_0
static const ClientUserinfoChanged_t changeClientUserinfo = (ClientUserinfoChanged_t)0x080F6506;
#elif COD_VERSION == COD2_1_2
static const ClientUserinfoChanged_t changeClientUserinfo = (ClientUserinfoChanged_t)0x080F8B1A;
#elif COD_VERSION == COD2_1_3
static const ClientUserinfoChanged_t changeClientUserinfo = (ClientUserinfoChanged_t)0x080F8C5E;
#endif

typedef int (*SV_ClientHasClientMuted_t)(int a1, int a2);
#if COD_VERSION == COD2_1_0
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809A01E;
#elif COD_VERSION == COD2_1_2
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809C0B6;
#elif COD_VERSION == COD2_1_3
static const SV_ClientHasClientMuted_t SV_ClientHasClientMuted = (SV_ClientHasClientMuted_t)0x0809C1FA;
#endif

typedef int (*Info_SetValueForKey_t)(char *s, const char *key, const char *value);
#if COD_VERSION == COD2_1_0
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B5FF6;
#elif COD_VERSION == COD2_1_2
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B848A;
#elif COD_VERSION == COD2_1_3
static const Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x080B85CE;
#endif

typedef char* (*Info_ValueForKey_t)(char *s, const char *key);
#if COD_VERSION == COD2_1_0
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B5B30;
#elif COD_VERSION == COD2_1_2
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B7FC4;
#elif COD_VERSION == COD2_1_3
static const Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x080B8108;
#endif

typedef short (*codscript_call_callback_entity_t)(int self, int callback, int params);
#if COD_VERSION == COD2_1_0
static const codscript_call_callback_entity_t codscript_call_callback_entity = (codscript_call_callback_entity_t)0x08118DF4; // search 'badMOD'
#elif COD_VERSION == COD2_1_2
static const codscript_call_callback_entity_t codscript_call_callback_entity = (codscript_call_callback_entity_t)0x0811B128;
#elif COD_VERSION == COD2_1_3
static const codscript_call_callback_entity_t codscript_call_callback_entity = (codscript_call_callback_entity_t)0x0811B284;
#endif

typedef int (*codscript_callback_finish_t)(short callback_handle);
#if COD_VERSION == COD2_1_0
static const codscript_callback_finish_t codscript_callback_finish = (codscript_callback_finish_t)0x08083B8E;
#elif COD_VERSION == COD2_1_2
static const codscript_callback_finish_t codscript_callback_finish = (codscript_callback_finish_t)0x0808410A;
#elif COD_VERSION == COD2_1_3
static const codscript_callback_finish_t codscript_callback_finish = (codscript_callback_finish_t)0x080841D6;
#endif

typedef int (*SVC_RemoteCommand_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x080951B4;
#elif COD_VERSION == COD2_1_2
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x080970CC;
#elif COD_VERSION == COD2_1_3
static const SVC_RemoteCommand_t SVC_RemoteCommand = (SVC_RemoteCommand_t)0x08097188;
#endif

typedef int (*SV_GetChallenge_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808BE54;
#elif COD_VERSION == COD2_1_2
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808D0C2;
#elif COD_VERSION == COD2_1_3
static const SV_GetChallenge_t SV_GetChallenge = (SV_GetChallenge_t)0x0808D18E;
#endif

typedef int (*SVC_Info_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x08093980;
#elif COD_VERSION == COD2_1_2
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x080952C4;
#elif COD_VERSION == COD2_1_3
static const SVC_Info_t SVC_Info = (SVC_Info_t)0x0809537C;
#endif

typedef int (*SVC_Status_t)(netadr_t from);
#if COD_VERSION == COD2_1_0
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08093288;
#elif COD_VERSION == COD2_1_2
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08094BCC;
#elif COD_VERSION == COD2_1_3
static const SVC_Status_t SVC_Status = (SVC_Status_t)0x08094C84;
#endif

typedef const char* (*NET_AdrToString_t)(netadr_t a);
#if COD_VERSION == COD2_1_0
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806AD14;
#elif COD_VERSION == COD2_1_2
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806B1DC;
#elif COD_VERSION == COD2_1_3
static const NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x0806B1D4;
#endif

typedef const char* (*Scr_Error_t)(const char *a1);
#if COD_VERSION == COD2_1_0
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x08084DB4;
#elif COD_VERSION == COD2_1_2
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x08085330;
#elif COD_VERSION == COD2_1_3
static const Scr_Error_t Scr_Error = (Scr_Error_t)0x080853FC;
#endif

#endif
