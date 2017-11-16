#include "gsc_player.hpp"

#if COMPILE_PLAYER == 1

void gsc_player_velocity_set(int id)
{
	vec3_t velocity;

	if ( ! stackGetParams("v", &velocity))
	{
		stackError("gsc_player_velocity_set() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_velocity_set() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	VectorCopy(velocity, entity->client->ps.velocity);
	stackPushBool(qtrue);
}

void gsc_player_velocity_add(int id)
{
	vec3_t velocity;

	if ( ! stackGetParams("v", &velocity))
	{
		stackError("gsc_player_velocity_add() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_velocity_add() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	VectorAdd(entity->client->ps.velocity, velocity, entity->client->ps.velocity);
	stackPushBool(qtrue);
}

void gsc_player_velocity_get(int id)
{
	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_velocity_add() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	stackPushVector(entity->client->ps.velocity);
}

void gsc_player_clientuserinfochanged(int id)
{
	ClientUserinfoChanged(id);
	stackPushBool(qtrue);
}

void gsc_player_get_userinfo(int id)
{
	char *key;

	if ( ! stackGetParams("s", &key))
	{
		stackError("gsc_player_get_userinfo() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_get_userinfo() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	char *val = Info_ValueForKey(client->userinfo, key);

	if (strlen(val))
		stackPushString(val);
	else
		stackPushUndefined();
}

void gsc_player_set_userinfo(int id)
{
	char *key, *value;

	if ( ! stackGetParams("ss", &key, &value))
	{
		stackError("gsc_player_set_userinfo() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_set_userinfo() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	Info_SetValueForKey(client->userinfo, key, value);
	stackPushBool(qtrue);
}

void gsc_player_button_ads(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_ads() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.buttons & KEY_MASK_ADS_MODE ? qtrue : qfalse);
}

void gsc_player_button_left(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_left() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.rightmove == KEY_MASK_MOVELEFT ? qtrue : qfalse);
}

void gsc_player_button_right(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_right() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.rightmove == KEY_MASK_MOVERIGHT ? qtrue : qfalse);
}

void gsc_player_button_forward(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_forward() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.forwardmove == KEY_MASK_FORWARD ? qtrue : qfalse);
}

void gsc_player_button_back(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_back() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.forwardmove == KEY_MASK_BACK ? qtrue : qfalse);
}

void gsc_player_button_leanleft(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_leanleft() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.buttons & KEY_MASK_LEANLEFT ? qtrue : qfalse);
}

void gsc_player_button_leanright(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_leanright() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.buttons & KEY_MASK_LEANRIGHT ? qtrue : qfalse);
}

void gsc_player_button_reload(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_reload() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.buttons & KEY_MASK_RELOAD ? qtrue : qfalse);
}

void gsc_player_button_jump(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_jump() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.buttons & KEY_MASK_JUMP ? qtrue : qfalse);
}

void gsc_player_button_frag(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_frag() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.buttons & KEY_MASK_FRAG ? qtrue : qfalse);
}

void gsc_player_button_smoke(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_smoke() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushBool(client->lastUsercmd.buttons & KEY_MASK_SMOKE ? qtrue : qfalse);
}

void gsc_player_stance_get(int id)
{
	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_stance_get() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	if (entity->s.eFlags & EF_CROUCHING)
		stackPushString("duck");
	else if (entity->s.eFlags & EF_PRONE)
		stackPushString("lie");
	else
		stackPushString("stand");
}

void gsc_player_stance_set(int id)
{
	char *stance;

	if ( ! stackGetParams("s", &stance))
	{
		stackError("gsc_player_stance_set() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_stance_set() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	int event;

	if (strcmp(stance, "stand") == 0)
		event = EV_STANCE_FORCE_STAND;
	else if (strcmp(stance, "crouch") == 0)
		event = EV_STANCE_FORCE_CROUCH;
	else if (strcmp(stance, "prone") == 0)
		event = EV_STANCE_FORCE_PRONE;
	else
	{
		stackError("gsc_player_stance_set() invalid argument '%s'. Valid arguments are: 'stand' 'crouch' 'prone'", stance);
		stackPushUndefined();
		return;
	}

	G_AddPredictableEvent(entity, event, 0);
	stackPushBool(qtrue);
}

void gsc_player_spectatorclient_get(int id)
{
	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_stance_set() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	if (entity->client->spectatorClient == -1)
		stackPushUndefined();
	else
		stackPushEntity(&g_entities[entity->client->spectatorClient]);
}

void gsc_player_getip(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_getip() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	char tmp[16];
	snprintf(tmp, sizeof(tmp), "%d.%d.%d.%d", client->netchan.remoteAddress.ip[0], client->netchan.remoteAddress.ip[1], client->netchan.remoteAddress.ip[2], client->netchan.remoteAddress.ip[3]);

	stackPushString(tmp);
}

void gsc_player_getping(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_getping() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushInt(client->ping);
}

void gsc_player_clientcommand(int id)
{
	ClientCommand(id);
	stackPushBool(qtrue);
}

void gsc_player_getlastconnecttime(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_getlastconnecttime() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushInt(client->lastConnectTime);
}

void gsc_player_getlastmsg(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_getlastmsg() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushInt(svs.time - client->lastPacketTime);
}

void gsc_player_getclientstate(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_getclientstate() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushInt(client->state);
}

void gsc_player_addresstype(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_addresstype() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	stackPushInt(client->netchan.remoteAddress.type);
}

void gsc_player_renameclient(int id)
{
	char *name;

	if ( ! stackGetParams("s", &name))
	{
		stackError("gsc_player_renameclient() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (strlen(name) > 32)
	{
		stackError("gsc_player_renameclient() player name is longer than 32 characters", id);
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_renameclient() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	Info_SetValueForKey(client->userinfo, "name", name);
	strcpy(client->name, name);

	stackPushBool(qtrue);
}

void gsc_player_outofbandprint(int id)
{
	char *cmd;

	if ( ! stackGetParams("s", &cmd))
	{
		stackError("gsc_player_outofbandprint() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_outofbandprint() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	NET_OutOfBandPrint(NS_SERVER, client->netchan.remoteAddress, cmd);
	stackPushBool(qtrue);
}

void gsc_player_connectionlesspacket(int id)
{
	char *cmd;

	if ( ! stackGetParams("s", &cmd))
	{
		stackError("gsc_player_connectionlesspacket() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_connectionlesspacket() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	byte bufData[131072];
	msg_t msg;

	MSG_Init(&msg, bufData, sizeof(bufData));

	MSG_WriteByte(&msg, svc_nop);
	MSG_WriteShort(&msg, 0);
	MSG_WriteLong(&msg, -1);
	MSG_WriteString(&msg, cmd);

	SV_ConnectionlessPacket(client->netchan.remoteAddress, &msg);
	stackPushBool(qtrue);
}

void gsc_player_resetnextreliabletime(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_resetnextreliabletime() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	client->floodprotect = 0;
	stackPushBool(qtrue);
}

void gsc_player_ismantling(int id)
{
	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_ismantling() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	if (entity->s.eFlags & EF_MANTLE)
		stackPushBool(qtrue);
	else
		stackPushBool(qfalse);
}

void gsc_player_isonladder(int id)
{
	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_isonladder() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	if (entity->client->ps.pm_flags & PMF_LADDER)
		stackPushBool(qtrue);
	else
		stackPushBool(qfalse);
}

void gsc_player_getjumpslowdowntimer(int id)
{
	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_isonladder() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	stackPushInt(entity->client->ps.pm_time);
}

void gsc_player_setg_speed(int id)
{
	int speed;

	if ( ! stackGetParams("i", &speed))
	{
		stackError("gsc_player_setg_speed() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_setg_speed() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	extern int player_g_speed[MAX_CLIENTS];

	if (speed < 0)
	{
		stackError("gsc_player_setg_speed() param must be equal or above zero");
		stackPushUndefined();
		return;
	}

	player_g_speed[id] = speed;
	stackPushBool(qtrue);
}

void gsc_player_setg_gravity(int id)
{
	int gravity;

	if ( ! stackGetParams("i", &gravity))
	{
		stackError("gsc_player_setg_gravity() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_setg_gravity() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	extern int player_g_gravity[MAX_CLIENTS];

	if (gravity < 0)
	{
		stackError("gsc_player_setg_gravity() param must be equal or above zero");
		stackPushUndefined();
		return;
	}

	player_g_gravity[id] = gravity;
	stackPushBool(qtrue);
}

void gsc_player_setweaponfiremeleedelay(int id)
{
	int delay;

	if ( ! stackGetParams("i", &delay))
	{
		stackError("gsc_player_setweaponfiremeleedelay() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (delay < 0)
	{
		stackError("gsc_player_setweaponfiremeleedelay() param must be equal or above zero");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_setweaponfiremeleedelay() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	entity->client->ps.weaponDelay = delay;
	stackPushBool(qtrue);
}

void gsc_player_set_anim(int id)
{
	char *animation;

	if ( ! stackGetParams("s", &animation))
	{
		stackError("gsc_player_set_anim() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	extern int custom_animation[MAX_CLIENTS];
	int animationIndex = 0;

	if (strcmp(animation, "none") != 0)
		animationIndex = BG_AnimationIndexForString(animation);

	custom_animation[id] = (animationIndex);
	stackPushBool(qtrue);
}

void gsc_player_getcooktime(int id)
{
	gentity_t *entity = &g_entities[id];

	if (entity->client == NULL)
	{
		stackError("gsc_player_getcooktime() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	stackPushInt(entity->client->ps.grenadeTimeLeft);
}

void gsc_entity_setalive(int id)
{
	int isAlive;

	if ( ! stackGetParams("i", &isAlive))
	{
		stackError("gsc_entity_setalive() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	entity->takedamage = isAlive;
	stackPushBool(qtrue);
}

void gsc_entity_setbounds(int id)
{
	float width, height;

	if ( ! stackGetParams("ff", &width, &height))
	{
		stackError("gsc_entity_setbounds() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	gentity_t *entity = &g_entities[id];

	vec3_t mins = {-height, -width, -width};
	vec3_t maxs = {width, width, height};

	VectorCopy(mins, entity->r.mins);
	VectorCopy(maxs, entity->r.maxs);

	stackPushBool(qtrue);
}

void gsc_kick_slot()
{
	int id;
	char* msg;

	if ( ! stackGetParams("is", &id, &msg))
	{
		stackError("gsc_kick_slot() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_kick_slot() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	if (client == NULL)
	{
		stackPushUndefined();
		return;
	}

	if (client->netchan.remoteAddress.type == NA_LOOPBACK)
	{
		stackPushUndefined();
		return;
	}

	SV_DropClient(client, msg);
	stackPushBool(qtrue);
}

void gsc_player_setguid(int id)
{
	int guid;

	if ( ! stackGetParams("i", &guid))
	{
		stackError("gsc_player_setguid() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_button_frag() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	client_t *client = &svs.clients[id];

	client->guid = guid;
	stackPushBool(qtrue);
}

void gsc_player_clienthasclientmuted(int id)
{
	int id2;

	if ( ! stackGetParams("i", &id2))
	{
		stackError("gsc_player_clienthasclientmuted() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_clienthasclientmuted() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	stackPushInt(SV_ClientHasClientMuted(id, id2));
}

void gsc_player_getlastgamestatesize(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_getlastgamestatesize() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	extern int gamestate_size[MAX_CLIENTS];
	stackPushInt(gamestate_size[id]);
}

void gsc_player_getfps(int id)
{
	if (id > MAX_CLIENTS)
	{
		stackError("gsc_player_getfps() entity %i is not a player", id);
		stackPushUndefined();
		return;
	}

	extern int clientfps[MAX_CLIENTS];
	stackPushInt(clientfps[id]);
}

#endif
