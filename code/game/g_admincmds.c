// Copyright (C) 2003 Caleb Gingles
//
#include "g_local.h"

// g_admincmds.c -- admin commands send by client

char *ConcatArgs( int start );
void G_ApplyKnockback( gentity_t *targ, vec3_t newDir, float knockback );
void Svcmd_CancelVote_f(void);
void LogExit( const char *string );
gspawn_t* G_SelectClientSpawnPoint ( int team );
void G_ResetGametypeEntities ( void );


void AdminCmd_Talk( gentity_t *ent, int type )
{
	char *text;

	if ( !ent )
	{
		return;
	}

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: talk <text>\n");
		return;
	}

	if ( !CheckCommand( &g_allowTalk, ent, type, qfalse, qtrue, qtrue ) )
	{
		return;
	}

	text = ConcatArgs( 2 );

	if ( type == 1 )
	{
		G_Say( ent, NULL, SAY_ADMIN_ALL, text, qfalse );
	}
	else if ( type == 2 )
	{
		G_Say( ent, NULL, SAY_REF_ALL, text, qfalse );
	}
	else if ( type == 3 )
	{
		G_Say( ent, NULL, SAY_CLAN_ALL, text, qfalse );
	}
}

void AdminCmd_Chat( gentity_t *ent, int type )
{
	char *text;

	if ( !ent )
	{
		return;
	}

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: chat <text>\n");
		return;
	}

	if ( !CheckCommand( &g_allowChat, ent, type, qfalse, qtrue, qfalse ) )
	{
		return;
	}

	text = ConcatArgs( 2 );

	if ( type == 1 )
	{
		G_Say( ent, NULL, SAY_ADMIN_ONLY, text, qfalse );
	}
	else if ( type == 2 )
	{
		G_Say( ent, NULL, SAY_REF_ONLY, text, qfalse );
	}
	else if ( type == 3 )
	{
		G_Say( ent, NULL, SAY_CLAN_ONLY, text, qfalse );
	}
}

void AdminCmd_Kick( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char			buffer[MAX_TOKEN_CHARS];
	int				id;
	char			*text;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: kick #id <optional text>\n");
		return;
	}

	if ( !CheckCommand( &g_allowKick, ent, type, qtrue, qtrue, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		if ( !CheckRank( ent, id, qfalse ) ) {
			continue;
		}

		if ( trap_Argc () > 3 ) {
			text = ConcatArgs( 3 );
		}
		else {
			text = "kicked by admin";
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: kick %i (%s) \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", id, g_entities[id].client->pers.netname, text );
		trap_SendConsoleCommand( EXEC_APPEND, va("clientkick %d %s\n", id, text ) );

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Ban( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char			buffer[MAX_TOKEN_CHARS];
	int				id, time;
	char			*text;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: ban #id #minutes <optional text>\n");
		return;
	}

	if ( !CheckCommand( &g_allowBan, ent, type, qtrue, qtrue, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		trap_Argv( 3, buffer, sizeof( buffer ) );
		time = atoi( buffer );

		if ( time == 0 )
		{
			time = g_defaultBanTime.integer;
		}

		if ( !CheckRank( ent, id, qfalse ) ) {
			continue;
		}

		if ( trap_Argc () > 4 ) {
			text = ConcatArgs( 4 );
		}
		else {
			text = va("banned by admin for %d minutes", time);
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: ban %i (%s) %i \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", id, g_entities[id].client->pers.netname, time, text );
		trap_SendConsoleCommand( EXEC_APPEND, va("banclient %d %d %s\n", id, time, text ) );

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Mute( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char			buffer[MAX_TOKEN_CHARS];
	int				id;
	gentity_t		*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: mute #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowMute, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( ent && id == ent->s.number && g_entities[id].client->sess.modData->adminref < 3 ) {
			G_PrintMessage( ent, va("^1>>^7 You can't %smute yourself.\n", g_entities[id].client->sess.muted?"un":""));
			continue;
		}

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( g_entities[id].client->sess.muted ) {
			G_AdminLogPrintf( "Admin (%i) action by %s: unmute %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );
			g_entities[id].client->sess.muted = qfalse;
			recondata[id].attributes &= (~RECON_MUTE);
			trap_SendServerCommand( -1, va("print \"%s%s^7 has been ^3UNMUTED^7 by %s%s\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
			trap_SendServerCommand( id_ent-g_entities, "cp \"You have been ^3UNMUTED^7 by admin\n\"");
			if ( currtag == 0 ) {	// Only play the sound once
				G_Sound(id_ent, CHAN_AUTO, G_SoundIndex( "sound/movers/doors/airlock_door02/airlock02_open.mp3" ) );
			}
		}
		else {
			G_AdminLogPrintf( "Admin (%i) action by %s: mute %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );
			g_entities[id].client->sess.muted = qtrue;
			recondata[id].attributes |= RECON_MUTE;
			recondata[id].finishtime = level.time;
			trap_SendServerCommand( -1, va("print \"%s%s^7 has been ^1MUTED^7 by %s%s\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
			trap_SendServerCommand( id_ent-g_entities, "cp \"You have been ^1MUTED^7 by admin\n\"");
			if ( currtag == 0 ) {	// Only play the sound once
				G_Sound(id_ent, CHAN_AUTO, G_SoundIndex( "sound/movers/doors/airlock_door02/airlock02_doorstop.mp3" ) );
			}
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_DoPenalty( gentity_t *ent, int time, int type )
{
	ent->client->ps.stats[STAT_WEAPONS] = 0;
	TossClientItems( ent );
	G_StartGhosting( ent );
	ent->client->ps.respawnTimer = time;
	ent->client->sess.modData->penalty = type;
	switch ( type )
	{
		case 5:
			recondata[ ent-g_entities ].attributes |= RECON_PENALTY5;
			break;
		case 4:
			recondata[ ent-g_entities ].attributes |= RECON_PENALTY4;
			break;
		case 3:
			recondata[ ent-g_entities ].attributes |= RECON_PENALTY3;
			break;
		case 2:
			recondata[ ent-g_entities ].attributes |= RECON_PENALTY2;
			break;
		case 1:
		default:
			recondata[ ent-g_entities ].attributes |= RECON_PENALTY1;
			break;
	}
	recondata[ ent-g_entities ].finishtime = time;
}

void AdminCmd_DoUnPenalty( gentity_t *ent )
{
	if ( ent->client->sess.modData->penalty )
	{
		ent->client->ps.respawnTimer = 0;
		ent->client->sess.modData->penalty = 0;
		G_StopFollowing( ent );
		G_StopGhosting( ent );
		trap_UnlinkEntity ( ent );
		ClientSpawn( ent );
		trap_SendServerCommand( -1, va("print \"%s%s^3 has been released from the penalty box.\n\"", level.teamData.teamcolor[ent->client->sess.team], ent->client->pers.netname));
		trap_SendServerCommand( ent-g_entities, "cp \"^3You have been released from the penalty box.\n^1Now behave!\n\n\"");
		recondata[ent->s.number].attributes = 0;
	}
}

void AdminCmd_Penalty( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id, time;
	gentity_t	*id_ent;
	char		*text;
	int			ptype;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: penalty #id <optional time> <optional reason>\n");
		return;
	}

	if ( !CheckCommand( &g_allowPenalty, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		type = 1;
		if ( trap_Argc () > 3 ) {
			trap_Argv( 3, buffer, sizeof( buffer ) );
			time = atoi( buffer );
			if ( !time ) {
				time = g_defaultPenaltyBoxTime.integer;
				strncpy( recondata[ ent-g_entities ].penaltyReason, ConcatArgs( 3 ), 63 );
				recondata[ ent-g_entities ].penaltyReason[63] = 0;
				ptype = 5;
			}
			else if ( trap_Argc () > 4 ) {
				strncpy( recondata[ ent-g_entities ].penaltyReason, ConcatArgs( 4 ), 63 );
				recondata[ ent-g_entities ].penaltyReason[63] = 0;
				ptype = 5;
			}
		}
		else {
			time = g_defaultPenaltyBoxTime.integer;
		}
		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( time > 3600 || time < 0 ) {
			G_PrintMessage( ent, "^1>>^7 Time length is too long.\n");
			return;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: penalty %i (%s) %i\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname, time );

		AdminCmd_DoPenalty( id_ent, level.time + (time * 1000), ptype );
		if ( currtag == 0 ) {	// Only play the sound once
			G_Sound(id_ent, CHAN_AUTO, G_SoundIndex( "sound/movers/doors/airlock_door02/airlock02_doorstop.mp3" ) );
		}
		trap_SendServerCommand( -1, va("cp \"@%s%s^3 has been thrown into\nthe ^1penalty box^3!\n\"", level.teamData.teamcolor[id_ent->client->sess.team], id_ent->client->pers.netname));
		trap_SendServerCommand( -1, va("print \"@%s%s^3 has been thrown into the ^1penalty box^3!\n\"", level.teamData.teamcolor[id_ent->client->sess.team], id_ent->client->pers.netname));

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_UnPenalty( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: unpenalty #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowUnPenalty, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( g_entities[id].client->sess.modData->penalty == 0 ) {
			G_PrintMessage( ent, "^1>>^7 That player is not currently in the penalty box.\n");
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: unpenalty %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		AdminCmd_DoUnPenalty( id_ent );

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Suspend( gentity_t *ent, int type )
{
	int adminval;
	int clientNum;

	if ( !ent )
	{
		return;
	}
	if ( level.match ) {
		G_PrintMessage( ent, "^1>>^7 This command is disabled during a match.\n");
		return;
	}

	clientNum = ent - g_entities;

	if ( ent->client->sess.modData->adminref > 1 ) {
		G_AdminLogPrintf( "Admin (%i) action by %s: suspend\n", type, ent->client->pers.netname );
		ent->client->sess.modData->adminref = -1;
		if ( !g_anonymousAdmin.integer )
		{
			trap_SendServerCommand( -1, va("cp \"^3%s^3 has suspended his admin status^7\n\"", ent->client->pers.netname));
			G_BroadcastSound("sound/movers/doors/airlock_door02/airlock02_doorstop.mp3");
		}
		else
		{
			G_PrintMessage( ent, "^3You have suspended your admin status\n");
		}
		ClientUserinfoChanged( clientNum );
		return;
	}
	else if ( ( adminval = CheckAdmin( clientNum ) ) != 0 ) {
		G_AdminLogPrintf( "Admin (%i) action by %s: unsuspend\n", type, ent->client->pers.netname );
		ent->client->sess.modData->adminref = adminval;
		ent->client->sess.teamkillDamage = 0;
		ent->client->sess.teamkillForgiveTime = 0;
		ent->client->sess.spawnkillDamage = 0;
		ent->client->sess.spawnkillForgiveTime = 0;
		if ( !g_anonymousAdmin.integer )
		{
			trap_SendServerCommand( -1, va("cp \"^3%s^3 has reinstated his admin status^7\n\"", ent->client->pers.netname));
			G_BroadcastSound("sound/movers/doors/airlock_door02/airlock02_doorstop.mp3");
		}
		else
		{
			G_PrintMessage( ent, "^3You have reinstated your admin status\n");
		}
		ClientUserinfoChanged( clientNum );
		if ( ent->client->sess.modData->adminref == 3 ) {
			if ( ent->client->sess.modData->penalty ) {
				AdminCmd_DoUnPenalty( ent );
			}
			if ( ent->client->sess.muted ) {
				ent->client->sess.muted = qfalse;
			}
		}
		return;
	}
	else {
		G_PrintMessage( ent, "^1>>^7 You are not an admin\n");
	}
}

void AdminCmd_DoPause( const char *message )
{
	gentity_t	*player;
	int			i;

	trap_SetConfigstring( CS_GAMETYPE_MESSAGE, va("%i,%s", level.time + 999999, message) );
	level.paused = qtrue;
	level.pauseTime = level.time;
	level.lastPauseTime = level.time;
	level.savedStartTime = level.startTime;
	level.intermissiontime = level.time;
	trap_SetConfigstring( CS_LEVEL_START_TIME, va("%i", level.time ) );

	for (i = 0; i < level.numConnectedClients; i++) 
	{
		player = &g_entities[level.sortedClients[i]];

		if ( player->client->pers.connected != CON_CONNECTED )
		{
			continue;
		}
		if ( player->client->sess.modData->paused ) 
		{
			continue;
		}

		player->client->sess.modData->paused = qtrue;

		VectorCopy( player->client->ps.origin, player->client->sess.modData->savedOrigin );
		VectorCopy( player->client->ps.viewangles, player->client->sess.modData->savedAngle );

		player->client->sess.modData->savedGhost = player->client->sess.ghost;
		player->client->sess.modData->savedSpectatorState = player->client->sess.spectatorState;
		player->client->sess.modData->savedSpectatorClient = player->client->sess.spectatorClient;

		player->client->sess.modData->savedData[0] = player->client->ps.pm_type;
		player->client->sess.modData->savedData[1] = player->client->ps.pm_flags;
		player->client->sess.modData->savedData[2] = player->client->ps.stats[STAT_GAMETYPE_ITEMS];
		player->client->sess.modData->savedData[3] = player->client->ps.stats[STAT_HEALTH];
		player->client->sess.modData->savedData[4] = player->client->ps.eFlags;
		player->client->sess.modData->savedData[5] = player->s.eFlags;
		player->client->sess.modData->savedData[6] = player->s.eType;
		player->client->sess.modData->savedData[7] = player->s.modelindex;
		player->client->sess.modData->savedData[8] = player->s.loopSound;
		player->client->sess.modData->savedData[9] = player->s.event;
		player->client->sess.modData->savedData[10] = player->r.contents;

//		G_StopFollowing( player );
//		G_StopGhosting ( player );
//		if ( G_IsClientDead ( player->client ) ) 
//		{
//			respawn ( player );
//		}
//		MoveClientToIntermission( player );
		player->client->ps.pm_type = PM_INTERMISSION;
	}
	SendScoreboardMessageToAllClients();
}

void AdminCmd_Pause( gentity_t *ent, int type )
{
	if ( !CheckCommand( &g_allowPause, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	if ( level.paused )
	{
		G_PrintMessage( ent, "^1>>^7 The game is already paused.\n");
		return;
	}

	AdminCmd_DoPause("Game is Paused");

	G_AdminLogPrintf( "Admin (%i) action by %s: pause\n", type, ent?ent->client->pers.netname:"rcon" );

	trap_SendServerCommand( -1, "print \"^3Game paused.\n\"");
	G_BroadcastSound("sound/misc/events/buzz01.mp3");
}

void AdminCmd_DoUnPause( void )
{
	gentity_t	*player;
	int			i;
	int			time;

	if ( !level.paused )
	{
		return;
	}

	time = level.time - level.pauseTime;
	level.startTime += time;

	if ( level.timeExtension ) level.timeExtension += time;
	if ( level.gametypeRoundTime ) level.gametypeRoundTime += time;
	if ( level.gametypeDelayTime ) level.gametypeDelayTime += time;
	if ( level.gametypeRespawnTime[0] ) level.gametypeRespawnTime[0] += time;
	if ( level.gametypeRespawnTime[1] ) level.gametypeRespawnTime[1] += time;
	if ( level.gametypeRespawnTime[2] ) level.gametypeRespawnTime[2] += time;
	if ( level.gametypeRespawnTime[3] ) level.gametypeRespawnTime[3] += time;
	if ( level.warmupTime )
	{
		level.warmupTime += time;
		trap_SetConfigstring( CS_WARMUP, va("%i", level.warmupTime) );
	}

	level.paused = qfalse;
	level.intermissiontime = 0;
	trap_SetConfigstring( CS_LEVEL_START_TIME, va("%i", level.startTime ) );

	for (i = 0; i < level.numConnectedClients; i++) 
	{
		player = &g_entities[level.sortedClients[i]];

		if ( player->client->pers.connected != CON_CONNECTED ) 
		{
			continue;
		}
		if ( !player->client->sess.modData->paused ) 
		{
//			G_StopFollowing( player );
//			G_StopGhosting( player );
//			trap_UnlinkEntity ( player );
//			ClientSpawn( player );
			continue;
		}

		if ( player->client->sess.modData->penalty )
		{
			player->client->ps.respawnTimer += time;
		}
		if ( player->client->sess.teamkillForgiveTime )
		{
			player->client->sess.teamkillForgiveTime += time;
		}

		player->client->sess.modData->paused = qfalse;

		player->client->sess.ghost = player->client->sess.modData->savedGhost;
		player->client->sess.spectatorState = player->client->sess.modData->savedSpectatorState;
		player->client->sess.spectatorClient = player->client->sess.modData->savedSpectatorClient;
		player->client->ps.stats[STAT_FROZEN] = 0;

		player->client->ps.pm_type = player->client->sess.modData->savedData[0];
		player->client->ps.pm_flags = player->client->sess.modData->savedData[1];
		player->client->ps.stats[STAT_GAMETYPE_ITEMS] = player->client->sess.modData->savedData[2];
		player->client->ps.stats[STAT_HEALTH] = player->client->sess.modData->savedData[3];
		player->client->ps.eFlags = player->client->sess.modData->savedData[4];
		player->s.eFlags = player->client->sess.modData->savedData[5];
		player->s.eType = player->client->sess.modData->savedData[6];
		player->s.modelindex = player->client->sess.modData->savedData[7];
		player->s.loopSound = player->client->sess.modData->savedData[8];
		player->s.event = player->client->sess.modData->savedData[9];
		player->r.contents = player->client->sess.modData->savedData[10];

//		TeleportPlayer(player, player->client->sess.modData->savedOrigin, player->client->sess.modData->savedAngle);

		trap_UnlinkEntity (player);
		VectorCopy ( player->client->sess.modData->savedOrigin, player->client->ps.origin );
//		AngleVectors( player->client->sess.modData->savedAngle, player->client->ps.velocity, NULL, NULL );
//		VectorScale( player->client->ps.velocity, 400, player->client->ps.velocity );
		player->client->ps.pm_time = 160;		// hold time
		player->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		player->client->ps.eFlags ^= EF_TELEPORT_BIT;
		SetClientViewAngle( player, player->client->sess.modData->savedAngle );
		BG_PlayerStateToEntityState( &player->client->ps, &player->s, qtrue );
		VectorCopy( player->client->ps.origin, player->r.currentOrigin );
		if ( !G_IsClientSpectating ( player->client ) ) 
		{
			trap_LinkEntity (player);
		}
	}
}

void AdminCmd_UnPause( gentity_t *ent, int type )
{
	if ( !CheckCommand( &g_allowUnPause, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	if ( !level.paused )
	{
		G_PrintMessage( ent, "^1>>^7 The game is not currently paused.\n");
		return;
	}

	level.unpauseTime = level.time;

	G_AdminLogPrintf( "Admin (%i) action by %s: unpause\n", type, ent?ent->client->pers.netname:"rcon" );
//	trap_SetConfigstring( CS_GAMETYPE_MESSAGE, "1, " );

	trap_SendServerCommand( -1, "print \"^3Game unpaused.\n\"");
//	G_BroadcastSound("sound/misc/events/buzz02.wav");
}

void AdminCmd_Strike( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: strike #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowStrike, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientSpectating ( id_ent->client ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: strike %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		id_ent->flags &= ~FL_GODMODE;
		id_ent->client->ps.stats[STAT_HEALTH] = id_ent->health = -999;
		player_die (id_ent, id_ent, /* ent?ent:id_ent */ NULL, 100000, MOD_ADMIN_STRIKE, HL_NONE, vec3_origin );

		trap_SendServerCommand( -1, va("print \"%s%s^7 was shown the light by %s%s.\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/ambience/generic/thunder_loud.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Slap( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id, slaptype;
	gentity_t	*id_ent;
	vec3_t		dir, kick;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: slap #id <optional type 1-5>\n" );
		return;
	}

	if ( !CheckCommand( &g_allowSlap, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		if ( trap_Argc () > 3 ) {
			trap_Argv( 3, buffer, sizeof( buffer ) );
			slaptype = atoi( buffer );
		}
		else {
			slaptype = 1;
		}
		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		switch( slaptype )
		{
			case 1:
				trap_SendServerCommand( -1, va("print \"%s%s^7 was schooled by %s%s^7's telephone receiver!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
				if ( currtag == 0 ) {	// Only play the sound once
					G_BroadcastSound( "sound/misc/confused/phone_shot.mp3" );
				}
				break;
			case 2:
				trap_SendServerCommand( -1, va("print \"%s%s^7 was hit by %s%s^7's saucepan!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
				if ( currtag == 0 ) {	// Only play the sound once
					G_BroadcastSound( "sound/misc/confused/metal_break.mp3" );
				}
				break;
			case 3:
				trap_SendServerCommand( -1, va("print \"%s%s^7 was clubbed over the head by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
				if ( currtag == 0 ) {	// Only play the sound once
					G_BroadcastSound( "sound/misc/confused/wood_break.mp3" );
				}
				break;
			case 4:
				trap_SendServerCommand( -1, va("print \"%s%s^7 was slapped by %s%s^7's cold fish!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
				if ( currtag == 0 ) {	// Only play the sound once
					G_BroadcastSound( "sound/misc/events/hk_hit02.mp3" );
				}
				break;
			case 5:
				trap_SendServerCommand( -1, va("print \"%s%s^7 was pinged by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
				if ( currtag == 0 ) {	// Only play the sound once
					G_BroadcastSound( "sound/misc/events/elev_beep.mp3" );
				}
				break;
			default:
				G_PrintMessage( ent, "^1>>^7 Invalid slap type.\n" );
				return;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: slap %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		VectorCopy( id_ent->client->ps.viewangles, dir );
		dir[0] = dir[2] = 0;
		AngleVectors( dir, kick, NULL, NULL);
		VectorInverse( kick );

		id_ent->client->ps.stats[STAT_ARMOR] -= 5;
		id_ent->client->ps.stats[STAT_HEALTH] -= 5;
		id_ent->health -= 5;
		id_ent->client->ps.velocity[0] = 0;
		id_ent->client->ps.velocity[1] = 0;

		if ( id_ent->health <= 0 )
		{
			player_die (id_ent, id_ent, /* ent?ent:id_ent */ NULL, 100000, MOD_ADMIN_SLAP, HL_NONE, vec3_origin );
		}

		G_ApplyKnockback( id_ent, kick, 400 );

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_DoStrip( int id )
{
	g_entities[id].client->ps.zoomFov = 0;
	g_entities[id].client->ps.pm_flags &= ~PMF_ZOOM_FLAGS;
	g_entities[id].client->ps.weapon = WP_KNIFE;
	g_entities[id].client->ps.weaponstate = WEAPON_READY;
	g_entities[id].client->ps.ammo[ weaponData[WP_KNIFE].attack[ATTACK_NORMAL].ammoIndex ] = 0;
	g_entities[id].client->ps.stats[STAT_WEAPONS] = 1 << WP_KNIFE;
	g_entities[id].client->noOutfittingChange = qtrue;
	recondata[id].attributes |= RECON_STRIP;
	recondata[id].finishtime = level.time;
}

void AdminCmd_Strip( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: strip #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowStrip, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: strip %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		AdminCmd_DoStrip( id );

		trap_SendServerCommand( -1, va("print \"%s%s^7 was just stripped of all his weapons by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/npc/air1/guard02/laughs.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_ForceTeam( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: forceteam #id <team>\n");
		return;
	}

	if ( !CheckCommand( &g_allowSwitch, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		trap_Argv( 3, buffer, sizeof( buffer ) );
		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: forceteam %i (%s) \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname, buffer );

		SetTeam( id_ent, buffer, NULL );

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Switch( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: switch #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowSwitch, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		level.override = qtrue;
		if ( id_ent->client->sess.team == TEAM_RED ) {
			G_AdminLogPrintf( "Admin (%i) action by %s: forceteam %i (%s) \"blue\"\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );
			SetTeam( id_ent, "blue", NULL );
		}
		else {
			G_AdminLogPrintf( "Admin (%i) action by %s: forceteam %i (%s) \"red\"\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );
			SetTeam( id_ent, "red", NULL );
		}
		level.override = qfalse;

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_CancelVote( gentity_t *ent, int type )
{
	if ( !level.voteTime ) {
		G_PrintMessage( ent, "^1>>^7 There is no vote in progress.\n");
		return;
	}

	if ( !CheckCommand( &g_allowCancelVote, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: cancelvote\n", type, ent?ent->client->pers.netname:"rcon" );
	Svcmd_CancelVote_f();
}

void AdminCmd_PassVote( gentity_t *ent, int type )
{
	if ( !level.voteTime ) {
		G_PrintMessage( ent, "^1>>^7 There is no vote in progress.\n");
		return;
	}

	if ( !CheckCommand( &g_allowPassVote, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: passvote\n", type, ent?ent->client->pers.netname:"rcon" );
	trap_SendServerCommand( -1, "print \"^5Vote passed by admin.^7\n\"" );
	level.voteYes = level.numVotingClients;
	level.voteNo = 0;
	trap_SetConfigstring( CS_VOTE_YES, va("%i", level.numVotingClients ) );
	trap_SetConfigstring( CS_VOTE_NO, "0" );
}

void AdminCmd_Gametype( gentity_t *ent, int type )
{
	char			buffer[MAX_TOKEN_CHARS];

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: gametype <type text>\n");
		return;
	}

	if ( !CheckCommand( &g_allowGametype, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );

	G_AdminLogPrintf( "Admin (%i) action by %s: g_gametype %s\n", type, ent?ent->client->pers.netname:"rcon", buffer );
	PrintCommand( type, va("^7Changing game type to ^3%s", buffer ) );

	level.voteExecuteTime = level.time + 2000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "g_gametype %s\n", buffer );
}

void AdminCmd_MapRestart( gentity_t *ent, int type )
{
	char	buffer[MAX_TOKEN_CHARS];
	int		time;

	if ( !CheckCommand( &g_allowMapRestart, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	time = 0;
	if ( trap_Argc () > 2 ) {
		trap_Argv( 2, buffer, sizeof( buffer ) );
		time = atoi( buffer );
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: map_restart\n", type, ent?ent->client->pers.netname:"rcon" );
	PrintCommand( type, "^7Map restart" );

	level.voteExecuteTime = level.time + 2000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "map_restart %i\n", time );
}

void AdminCmd_Mapcycle( gentity_t *ent, int type )
{
	if ( !CheckCommand( &g_allowMapcycle, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: mapcycle\n", type, ent?ent->client->pers.netname:"rcon" );
	PrintCommand( type, "^7Next map" );

	level.voteExecuteTime = level.time + 2000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "mapcycle\n" );
}

void AdminCmd_SetMap( gentity_t *ent, int type )
{
	char	mapname[MAX_TOKEN_CHARS];
	char	gametype[MAX_TOKEN_CHARS];

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: map <map name> <optional gametype>\n");
		return;
	}

	if ( !CheckCommand( &g_allowSetMap, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, mapname, sizeof( mapname ) );

	gametype[0] = 0;
	if ( trap_Argc () > 3 ) {
		trap_Argv( 3, gametype, sizeof( gametype ) );
		trap_SendConsoleCommand( EXEC_APPEND, va("g_gametype %s\n", gametype ) );
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: map %s (%s)\n", type, ent?ent->client->pers.netname:"rcon", mapname, gametype[0]?gametype:g_gametype.string );
	PrintCommand( type, va("^7Changing map to ^3%s ^5(%s)", mapname, gametype[0]?gametype:g_gametype.string ) );

	level.voteExecuteTime = level.time + 2000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "map %s\n", mapname );
}

void AdminCmd_ExtendTime( gentity_t *ent, int type )
{
	char	buffer[MAX_TOKEN_CHARS];
	int		time;

	if ( !CheckCommand( &g_allowExtendTime, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	time = 15;
	if ( trap_Argc () > 2 ) {
		trap_Argv( 2, buffer, sizeof( buffer ) );
		time = atoi( buffer );
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: extendtime %d\n", type, ent?ent->client->pers.netname:"rcon", time );
	PrintCommand( type, va("^7Timelimit extended by ^3%d^7 minutes", time ) );

	level.voteExecuteTime = level.time + 2000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "extendtime %d\n", time );
}

void AdminCmd_Warmup( gentity_t *ent, int type )
{
	char	buffer[MAX_TOKEN_CHARS];
	char	*value;
	int		i;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: warmup <on/off>\n");
		return;
	}

	if ( !CheckCommand( &g_allowWarmup, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	value = "ON";
	i = 1;
	trap_Argv( 2, buffer, sizeof( buffer ) );
	if ( !Q_stricmp( buffer, "off" ) || !Q_stricmp( buffer, "0" ) )
	{
		value = "OFF";
		i = 0;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: warmup %s\n", type, ent?ent->client->pers.netname:"rcon", value );
	PrintCommand( type, va("^7Warmup set to ^3%s", value ) );

	level.voteExecuteTime = level.time + 1000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "g_doWarmup %i\n", i );
}

void AdminCmd_FriendlyFire( gentity_t *ent, int type )
{
	char	buffer[MAX_TOKEN_CHARS];
	char	*value;
	int		i;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: friendlyfire <on/off>\n");
		return;
	}

	if ( !CheckCommand( &g_allowFriendlyFire, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	value = "ON";
	i = 1;
	trap_Argv( 2, buffer, sizeof( buffer ) );
	if ( !Q_stricmp( buffer, "off" ) || !Q_stricmp( buffer, "0" ) )
	{
		value = "OFF";
		i = 0;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: friendlyfire %s\n", type, ent?ent->client->pers.netname:"rcon", value );
	PrintCommand( type, va("^7Friendly fire set to ^3%s", value ) );

	level.voteExecuteTime = level.time + 1000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "g_friendlyfire %i\n", i );
}

void AdminCmd_Timelimit( gentity_t *ent, int type )
{
	char	buffer[MAX_TOKEN_CHARS];
	int		time;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: timelimit #time\n");
		return;
	}

	if ( !CheckCommand( &g_allowTimelimit, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	time = atoi( buffer );

	G_AdminLogPrintf( "Admin (%i) action by %s: timelimit %d\n", type, ent?ent->client->pers.netname:"rcon", time );
	PrintCommand( type, va("^7Time limit set to ^3%d", time ) );

	level.voteExecuteTime = level.time + 2000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "timelimit %d\n", time );
}

void AdminCmd_Scorelimit( gentity_t *ent, int type )
{
	char	buffer[MAX_TOKEN_CHARS];
	int		score;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: scorelimit #score\n");
		return;
	}

	if ( !CheckCommand( &g_allowScorelimit, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	score = atoi( buffer );

	G_AdminLogPrintf( "Admin (%i) action by %s: scorelimit %d\n", type, ent?ent->client->pers.netname:"rcon", score );
	PrintCommand( type, va("^7Score limit set to ^3%d", score ) );

	level.voteExecuteTime = level.time + 2000;
	level.voteTime = 0;
	trap_SetConfigstring( CS_VOTE_TIME, "" );
	Com_sprintf( level.voteString, sizeof( level.voteString ), "scorelimit %d\n", score );
}

void AdminCmd_DoSwapTeams( gentity_t *ent )
{
	int			i;
	gentity_t	*player;
	int			teamscore;

	for (i = 0; i < level.numConnectedClients; i++) 
	{
		player = &g_entities[level.sortedClients[i]];
		if ( player->client->pers.connected != CON_CONNECTED && player->client->pers.connected != CON_CONNECTING )
		{
			continue;
		}
		if ( player->client->sess.team == TEAM_FREE || player->client->sess.team == TEAM_SPECTATOR ) 
		{
			continue;
		}
		if ( ent && ent->client->sess.modData->adminref < 3 && player->client->sess.modData->adminref == 3 && g_sysopImmuneToAdmin.integer ) {
			continue;
		}
		player->client->ps.stats[STAT_WEAPONS] = 0;
		TossClientItems( player );
		G_StartGhosting( player );
		if ( player->client->sess.team == TEAM_RED ) {
			player->client->sess.team = TEAM_BLUE;
		}
		else {
			player->client->sess.team = TEAM_RED;
		}

		G_SendExtraTeamInfo( player );

		if ( player->client->sess.modData->penalty )
		{
			continue;
		}

		player->client->pers.identity = NULL;
		ClientUserinfoChanged( level.sortedClients[i] );

		CalculateRanks();

		G_StopFollowing( player );
		G_StopGhosting( player );
		trap_UnlinkEntity ( player );
		ClientSpawn( player );
	}

	teamscore = level.teamScores[TEAM_RED];
	level.teamScores[TEAM_RED] = level.teamScores[TEAM_BLUE];
	level.teamScores[TEAM_BLUE] = teamscore;
}

void AdminCmd_SwapTeams( gentity_t *ent, int type )
{
	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowSwapTeams, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

//	if ( level.teamRedLocked || level.teamBlueLocked )
//	{
//		G_PrintMessage( ent, "^1>>^7 Teams are locked.\n");
//		return;
//	}

	G_AdminLogPrintf( "Admin (%i) action by %s: swapteams\n", type, ent?ent->client->pers.netname:"rcon" );
	PrintCommand( type, "^7Swapping teams" );
	G_BroadcastSound("sound/misc/events/tut_lift02.mp3");

	AdminCmd_DoSwapTeams( ent );
}

void AdminCmd_DoShuffleTeams( gentity_t *ent )
{
	int			i;
	int			rnd;
	int			red;
	int			blue;
	int			team;
	int			count;
	gentity_t	*player;

	red = 0;
	blue = 0;
	for (i = 0; i < level.numConnectedClients; i++) 
	{
		player = &g_entities[level.sortedClients[i]];

		if ( player->client->pers.connected != CON_CONNECTED && player->client->pers.connected != CON_CONNECTING )
		{
			continue;
		}
		if ( player->client->sess.team == TEAM_FREE || player->client->sess.team == TEAM_SPECTATOR ) 
		{
			continue;
		}
		if ( ent && ent->client->sess.modData->adminref < 3 && player->client->sess.modData->adminref == 3 && g_sysopImmuneToAdmin.integer ) {
//			if ( player->client->sess.team == TEAM_RED ) {
//				red++;
//			}
//			else {
//				blue++;
//			}
			continue;
		}
		player->client->ps.stats[STAT_WEAPONS] = 0;
		TossClientItems( player );
		G_StartGhosting( player );
//		if ( blue > red && (blue - red) > 2 ) {
//			player->client->sess.team = TEAM_RED;
//			red++;
//		}
//		else if ( red > blue && (red - blue) > 2 ) {
//			player->client->sess.team = TEAM_BLUE;
//			blue++;
//		}
//		else
//		{
			rnd = rand() % 10;
			if ( rnd < 5 ) {
				player->client->sess.team = TEAM_BLUE;
//				blue++;
			}
			else {
				player->client->sess.team = TEAM_RED;
//				red++;
			}
//		}
/**/
		G_SendExtraTeamInfo( player );

		if ( player->client->sess.modData->penalty )
		{
			continue;
		}

		player->client->pers.identity = NULL;
		ClientUserinfoChanged( level.sortedClients[i] );

		CalculateRanks();
/*
		G_StopFollowing( player );
		G_StopGhosting( player );
		trap_UnlinkEntity ( player );
		ClientSpawn( player );
*/
	}

	red = TeamCount( -1, TEAM_RED, NULL );
	blue = TeamCount( -1, TEAM_BLUE, NULL );
	if ( red - blue > 1 )
	{
		count = (red - blue) / 2;
		team = TEAM_RED;
	}
	else if ( blue - red > 1 )
	{
		count = (blue - red) / 2;
		team = TEAM_BLUE;
	}
	else
	{
		return;
	}

	AdminCmd_DoEvenTeams( ent, team, count );
}

void AdminCmd_ShuffleTeams( gentity_t *ent, int type )
{
	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowShuffleTeams, ent, type, qtrue, qfalse, qtrue ) )
	{
		return;
	}

	if ( level.teamRedLocked || level.teamBlueLocked )
	{
		G_PrintMessage( ent, "^1>>^7 Teams are locked.\n");
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: shuffleteams\n", type, ent?ent->client->pers.netname:"rcon" );
	PrintCommand( type, "^7Shuffling teams" );

	AdminCmd_DoShuffleTeams( ent );
}

void AdminCmd_EndMap( gentity_t *ent, int type )
{
	if ( !CheckCommand( &g_allowEndMap, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: endmap\n", type, ent?ent->client->pers.netname:"rcon" );
	PrintCommand( type, "^7Ending map" );

	level.endMap = qtrue;
}

void AdminCmd_LockTeam( gentity_t *ent, int type )
{
	char		buffer[MAX_TOKEN_CHARS];
	int			i;
	gentity_t	*player;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: lock <team>\n");
		return;
	}

	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowLockTeam, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );

	if ( Q_stricmp ( buffer, level.teamData.redName ) == 0 || Q_stricmp ( buffer, "red" ) == 0 )
	{
		if ( level.teamRedLocked )
		{
			level.teamRedLocked = qfalse;
			G_AdminLogPrintf( "Admin (%i) action by %s: unlock \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", buffer );
			PrintCommand( type, va("^7Unlock %s ^7team", level.teamData.redName) );
		}
		else
		{
			level.teamRedLocked = qtrue;
			G_AdminLogPrintf( "Admin (%i) action by %s: lock \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", buffer );
			PrintCommand( type, va("^7Lock %s ^7team", level.teamData.redName) );
		}
	}
	else if ( Q_stricmp ( buffer, level.teamData.blueName ) == 0 || Q_stricmp ( buffer, "blue" ) == 0 )
	{
		if ( level.teamBlueLocked )
		{
			level.teamBlueLocked = qfalse;
			G_AdminLogPrintf( "Admin (%i) action by %s: unlock \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", buffer );
			PrintCommand( type, va("^7Unlock %s ^7team", level.teamData.blueName) );
		}
		else
		{
			level.teamBlueLocked = qtrue;
			G_AdminLogPrintf( "Admin (%i) action by %s: lock \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", buffer );
			PrintCommand( type, va("^7Lock %s ^7team", level.teamData.blueName) );
		}
	}

	for (i = 0; i < level.numConnectedClients; i++) 
	{
		player = &g_entities[level.sortedClients[i]];

		if ( player->client->pers.connected != CON_CONNECTED ) 
		{
			continue;
		}
		if ( player->client->sess.team != TEAM_SPECTATOR ) 
		{
			continue;
		}
		if ( player->client->sess.spectatorState == SPECTATOR_FOLLOW && g_entities[player->client->sess.spectatorClient].client->sess.team == TEAM_RED && level.teamRedLocked && !player->client->sess.modData->invitedRed )
		{
			Cmd_FollowCycle_f (player, 1);
		}
		if ( player->client->sess.spectatorState == SPECTATOR_FOLLOW && g_entities[player->client->sess.spectatorClient].client->sess.team == TEAM_BLUE && level.teamBlueLocked && !player->client->sess.modData->invitedBlue )
		{
			Cmd_FollowCycle_f (player, 1);
		}
	}
}

void AdminCmd_SuspendAll( gentity_t *ent, int type )
{
	int			i;
	gentity_t	*player;
	int			adminref;

	if ( !CheckCommand( &g_allowSuspendAll, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: suspendall\n", type, ent?ent->client->pers.netname:"rcon" );
	PrintCommand( type, "^7Suspend All" );

	adminref = ent?ent->client->sess.modData->adminref:3;

	for (i = 0; i < level.numConnectedClients; i++) 
	{
		player = &g_entities[level.sortedClients[i]];

		if ( player->client->pers.connected != CON_CONNECTED ) 
		{
			continue;
		}

		if ( player->client->sess.modData->adminref < 2 )
		{
			continue;
		}

		if ( adminref < 3 && player->client->sess.modData->adminref == 3 && g_sysopImmuneToAdmin.integer && !ent->client->sess.modData->moderator )
		{
			continue;
		}

		player->client->sess.modData->adminref = -1;
		ClientUserinfoChanged( player->client - level.clients );
	}
}

void AdminCmd_Fry( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: strip #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowFry, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: fry %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		id_ent->client->sess.modData->fryFireTime = level.time - 2000;
		id_ent->client->sess.modData->fryPainTime = level.time - 2000;
		id_ent->client->sess.modData->fryInflictor = /* ent?ent:id_ent */ NULL;
		recondata[id].attributes |= RECON_FRY;
		recondata[id].finishtime = level.time;

		trap_SendServerCommand( -1, va("print \"%s%s^7 is being burned to a crisp by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/weapons/explosions/explosion_flame.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Launch( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;
	vec3_t		dir;
	int			weapons;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: launch #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowLaunch, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: launch %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		weapons = id_ent->client->ps.stats[STAT_WEAPONS];
		id_ent->client->ps.stats[STAT_WEAPONS] = 0;
		TossClientItems( id_ent );
		id_ent->client->ps.stats[STAT_WEAPONS] = weapons;

		dir[0] = dir[1] = 0;
		dir[2] = 1;
		G_ApplyKnockback( id_ent, dir, 1000 );
		id_ent->client->ps.stats[STAT_ARMOR] /= 10;
		if ( id_ent->client->ps.stats[STAT_ARMOR] < 1 ) {
			id_ent->client->ps.stats[STAT_ARMOR] = 1;
		}
		id_ent->client->ps.stats[STAT_HEALTH] /= 10;
		if ( id_ent->client->ps.stats[STAT_HEALTH] < 1 ) {
			id_ent->client->ps.stats[STAT_HEALTH] = 1;
		}
		id_ent->health /= 10;
		if ( id_ent->health < 1 ) {
			id_ent->health = 1;
		}

		trap_SendServerCommand( -1, va("print \"%s%s^7 was tossed into the air by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/misc/events/missile_fly.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Explode( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;
	int			location;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: explode #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowExplode, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: explode %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		location = HL_FOOT_RT|HL_FOOT_LT|HL_LEG_UPPER_RT|HL_LEG_UPPER_LT|HL_LEG_LOWER_RT|HL_LEG_LOWER_LT|HL_HAND_RT|HL_HAND_LT|HL_ARM_RT|HL_ARM_LT|HL_HEAD|HL_WAIST|HL_BACK_RT|HL_BACK_LT|HL_BACK|HL_CHEST_RT|HL_CHEST_LT|HL_CHEST|HL_NECK;
		location |= HL_DISMEMBERBIT;
		G_Damage( id_ent, NULL, /* ent?ent:id_ent */ NULL, NULL, NULL, 9999, DAMAGE_NO_PROTECTION|DAMAGE_FORCE_GORE, MOD_ADMIN_EXPLODE, location);

		trap_SendServerCommand( -1, va("print \"%s%s^7 was blown to pieces by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/player/bullet_impacts/gore/disembowel01.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Plant( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: plant #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowPlant, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: plant %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		id_ent->client->ps.stats[STAT_PLANTED] = 1;
		recondata[id].attributes |= RECON_PLANT;
		recondata[id].finishtime = level.time;

		trap_SendServerCommand( -1, va("print \"%s%s^7 has been glued to the floor by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/npc/air1/guard02/laughs.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Telefrag( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;
	int			weapons;

	if ( !ent )
	{
		return;
	}

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: telefrag #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowTelefrag, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: telefrag %i (%s)\n", type, ent->client->pers.netname, id, id_ent->client->pers.netname );

		weapons = ent->client->ps.stats[STAT_WEAPONS];
		ent->client->ps.stats[STAT_WEAPONS] = 0;
		TossClientItems( ent );
		trap_UnlinkEntity (ent);
		VectorCopy ( id_ent->client->ps.origin, ent->client->ps.origin );
		ent->client->ps.pm_time = 160;		// hold time
		ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
		SetClientViewAngle( ent, id_ent->client->ps.viewangles );
		if ( !G_IsClientSpectating ( ent->client ) ) 
		{
			G_KillBoxAdm (ent);
		}
		BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		if ( !G_IsClientSpectating ( ent->client ) ) 
		{
			trap_LinkEntity (ent);
			ent->client->ps.stats[STAT_WEAPONS] = weapons;
		}

		trap_SendServerCommand( -1, va("print \"%s%s^7 was telefragged by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/pain_death/male/groan01.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Surrender( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: surrender #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowSurrender, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: surrender %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		g_entities[id].client->ps.weapon = 0;
		g_entities[id].client->ps.weaponstate = WEAPON_READY;
		g_entities[id].client->ps.stats[STAT_WEAPONS] = 0;
		g_entities[id].client->noOutfittingChange = qtrue;
		recondata[id].attributes |= RECON_SURRENDER;
		recondata[id].finishtime = level.time;

		trap_SendServerCommand( -1, va("print \"%s%s^7 was turned into a coward by %s%s!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/npc/civ/english/male/unarmed.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Respawn( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;
	gspawn_t	*spawnPoint;
	int			weapons;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: respawn #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowRespawn, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: respawn %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		spawnPoint = G_SelectClientSpawnPoint ( id_ent->client->sess.team );
		if ( spawnPoint )
		{
			weapons = id_ent->client->ps.stats[STAT_WEAPONS];
			id_ent->client->ps.stats[STAT_WEAPONS] = 0;
			TossClientItems( id_ent );
			trap_UnlinkEntity (id_ent);
			VectorCopy ( spawnPoint->origin, id_ent->client->ps.origin );
			id_ent->client->ps.pm_time = 160;		// hold time
			id_ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
			id_ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
			SetClientViewAngle( id_ent, spawnPoint->angles );
			if ( !G_IsClientSpectating ( id_ent->client ) ) 
			{
				G_KillBoxAdm (id_ent);
			}
			BG_PlayerStateToEntityState( &id_ent->client->ps, &id_ent->s, qtrue );
			VectorCopy( id_ent->client->ps.origin, id_ent->r.currentOrigin );
			if ( !G_IsClientSpectating ( id_ent->client ) ) 
			{
				trap_LinkEntity (id_ent);
				id_ent->client->ps.stats[STAT_WEAPONS] = weapons;
			}
		}

		trap_SendServerCommand( -1, va("print \"%s%s^7 was teleported home by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/npc/air1/guard02/laughs.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_DoBait( gentity_t *ent )
{
	gspawn_t	*spawnPoint;
	int			weapons;

	if ( ent->client->sess.team == TEAM_RED )
	{
		spawnPoint = G_SelectClientSpawnPoint ( TEAM_BLUE );
	}
	else
	{
		spawnPoint = G_SelectClientSpawnPoint ( TEAM_RED );
	}
	if ( spawnPoint )
	{
		weapons = ent->client->ps.stats[STAT_WEAPONS];
		ent->client->ps.stats[STAT_WEAPONS] = 0;
		TossClientItems( ent );
		trap_UnlinkEntity( ent );
		VectorCopy ( spawnPoint->origin, ent->client->ps.origin );
		ent->client->ps.pm_time = 160;		// hold time
		ent->client->ps.pm_flags |= PMF_TIME_KNOCKBACK;
		ent->client->ps.eFlags ^= EF_TELEPORT_BIT;
		SetClientViewAngle( ent, spawnPoint->angles );
		if ( !G_IsClientSpectating ( ent->client ) ) 
		{
			G_KillBoxAdm( ent );
		}
		BG_PlayerStateToEntityState( &ent->client->ps, &ent->s, qtrue );
		VectorCopy( ent->client->ps.origin, ent->r.currentOrigin );
		if ( !G_IsClientSpectating ( ent->client ) ) 
		{
			trap_LinkEntity( ent );
			ent->client->ps.stats[STAT_WEAPONS] = weapons;
		}

		ent->client->ps.weapon = 0;
		ent->client->ps.weaponstate = WEAPON_READY;
		ent->client->ps.stats[STAT_WEAPONS] = 0;

		ent->client->ps.stats[STAT_PLANTED] = 1;
		ent->client->noOutfittingChange = qtrue;
	}
	recondata[ent-g_entities].attributes |= RECON_BAIT;
	recondata[ent-g_entities].finishtime = level.time;
}

void AdminCmd_Bait( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: bait #id\n");
		return;
	}

	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowBait, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qtrue ) ) {
			continue;
		}

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: bait %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

		AdminCmd_DoBait( id_ent );

		trap_SendServerCommand( -1, va("print \"%s%s^7 was fed to the enemy by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/npc/air1/guard02/laughs.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_DoEvenTeams( gentity_t *ent, int team, int count )
{
	int			i;
	int			score;
	int			id;
	int			teamred;
	int			teamblue;
	gentity_t	*player;

	while ( count-- )
	{
		score = 999999;
		id = 0;
		for (i = 0; i < level.numConnectedClients; i++) 
		{
			player = &g_entities[level.sortedClients[i]];
			if ( player->client->pers.connected != CON_CONNECTED && player->client->pers.connected != CON_CONNECTING )
			{
				continue;
			}
			if ( player->client->sess.team == TEAM_FREE || player->client->sess.team == TEAM_SPECTATOR || player->client->sess.team != team )
			{
				continue;
			}
			if ( ent && ent->client->sess.modData->adminref < 3 && player->client->sess.modData->adminref == 3 && g_sysopImmuneToAdmin.integer ) {
				continue;
			}
			if ( player->s.number == level.redFlagCarrier || player->s.number == level.blueFlagCarrier )
			{
				continue;
			}

			if ( player->client->sess.modData->recondata->score < score )
			{
				score = player->client->sess.modData->recondata->score;
				id = player->s.number;
			}
		}

		player = g_entities + id;

		player->client->ps.stats[STAT_WEAPONS] = 0;
		TossClientItems( player );
		G_StartGhosting( player );
		if ( player->client->sess.team == TEAM_RED ) {
			player->client->sess.team = TEAM_BLUE;
		}
		else {
			player->client->sess.team = TEAM_RED;
		}

		G_SendExtraTeamInfo( player );

		if ( player->client->sess.modData->penalty )
		{
			continue;
		}

		player->client->pers.identity = NULL;
		ClientUserinfoChanged( id );

		CalculateRanks();

		G_StopFollowing( player );
		G_StopGhosting( player );
		trap_UnlinkEntity ( player );
		ClientSpawn( player );
	}
}

void AdminCmd_EvenTeams( gentity_t *ent, int type )
{
	int			i;
	int			score;
	int			id;
	int			team;
	int			teamred;
	int			teamblue;
	int			count;
	gentity_t	*player;

	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowEvenTeams, ent, type, qtrue, qfalse, qtrue ) )
	{
		return;
	}

	if ( level.teamRedLocked || level.teamBlueLocked )
	{
		G_PrintMessage( ent, "^1>>^7 Teams are locked.\n");
		return;
	}

	teamred = TeamCount( -1, TEAM_RED, NULL );
	teamblue = TeamCount( -1, TEAM_BLUE, NULL );
	if ( teamred - teamblue > 1 )
	{
		count = (teamred - teamblue) / 2;
		team = TEAM_RED;
	}
	else if ( teamblue - teamred > 1 )
	{
		count = (teamblue - teamred) / 2;
		team = TEAM_BLUE;
	}
	else
	{
		G_PrintMessage( ent, "^4>>^7 Teams are already even.\n");
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: eventeams\n", type, ent?ent->client->pers.netname:"rcon" );
	PrintCommand( type, "^7Evening teams" );

	AdminCmd_DoEvenTeams( ent, team, count );
}

void AdminCmd_Invite( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: invite #id <team>\n");
		return;
	}

	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowInvite, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );
	trap_Argv( 3, buffer, sizeof( buffer ) );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( G_IsClientDead ( id_ent->client ) ) {
			continue;
		}

		if ( Q_stricmp ( buffer, level.teamData.redName ) == 0 || Q_stricmp ( buffer, "red" ) == 0 )
		{
			id_ent->client->sess.modData->invitedRed = qtrue;
			G_PrintMessage( ent, va("^4>>^7 %s^7 has been invited to the %s team.\n", id_ent->client->pers.netname, level.teamData.redName));
		}
		else if ( Q_stricmp ( buffer, level.teamData.blueName ) == 0 || Q_stricmp ( buffer, "blue" ) == 0 )
		{
			id_ent->client->sess.modData->invitedRed = qtrue;
			G_PrintMessage( ent, va("^4>>^7 %s^7 has been invited to the %s team.\n", id_ent->client->pers.netname, level.teamData.blueName));
		}
		else
		{
			G_PrintMessage( ent, "^1>>^7 Unknown team specified.\n");
			return;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: invite %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_ForceSay( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;
	char		*text;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: forcesay #id <text>\n");
		return;
	}

	if ( !CheckCommand( &g_allowForceSay, ent, type, qfalse, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qfalse ) ) {
			continue;
		}

		text = ConcatArgs( 3 );

		G_Say( id_ent, NULL, SAY_ALL, text, qtrue );

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Swap( gentity_t *ent, int type )
{
	char		buffer[MAX_TOKEN_CHARS];
	int			id1;
	int			id2;
	gentity_t	*id_ent1;
	gentity_t	*id_ent2;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: swap #id1 #id2\n");
		return;
	}

	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowSwap, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id1 = ParseID( ent, buffer );

	if ( !CheckID( ent, id1 ) ) {
		return;
	}

	id_ent1 = g_entities + id1;
	trap_Argv( 3, buffer, sizeof( buffer ) );
	id2 = ParseID( ent, buffer );

	if ( !CheckID( ent, id2 ) ) {
		return;
	}

	id_ent2 = g_entities + id2;

	if ( !CheckRank( ent, id1, qtrue ) || !CheckRank( ent, id2, qtrue ) ) {
		return;
	}

	if ( id_ent1->client->sess.team == id_ent2->client->sess.team ) {
		G_PrintMessage( ent, "^1>>^7 They are on the same team.\n");
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: swap %i (%s) %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id1, id_ent1->client->pers.netname, id2, id_ent2->client->pers.netname );

	if ( id_ent1->client->sess.team == TEAM_RED ) {
		SetTeamInstant( id_ent1, TEAM_BLUE, qtrue );
		SetTeamInstant( id_ent2, TEAM_RED, qtrue );
	}
	else {
		SetTeamInstant( id_ent1, TEAM_RED, qtrue );
		SetTeamInstant( id_ent2, TEAM_BLUE, qtrue );
	}
}

void AdminCmd_DoClanVsAll( gentity_t *ent, char *buffer )
{
	int			team1, team2;
	int			i;
	gclient_t	*cl;

	if ( !Q_stricmp( buffer, level.teamData.redName ) || !Q_stricmp( buffer, "red" ) || !Q_stricmp( buffer, "r" ) )
	{
		team1 = TEAM_RED;
		team2 = TEAM_BLUE;
	} 
	else if ( !Q_stricmp( buffer, level.teamData.blueName ) || !Q_stricmp( buffer, "blue" ) || !Q_stricmp( buffer, "b" ) ) 
	{
		team1 = TEAM_BLUE;
		team2 = TEAM_RED;
	}
	else
	{
		return;
	}

	for (i = 0; i < g_maxclients.integer; i++)
	{
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED && cl->pers.connected != CON_CONNECTING ) 
		{
			continue;
		}
		if ( cl->sess.team == TEAM_FREE || cl->sess.team == TEAM_SPECTATOR ) 
		{
			continue;
		}

		if ( cl->sess.modData->clan ) {
			SetTeamInstant( &g_entities[i], team1, qfalse );
		}
		else {
			SetTeamInstant( &g_entities[i], team2, qfalse );
		}
	}
}

void AdminCmd_ClanVsAll( gentity_t *ent, int type )
{
	char		buffer[MAX_TOKEN_CHARS];
	int			team1, team2;
	int			i;
	gclient_t	*cl;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: clanvsall <team>\n");
		return;
	}

	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowClanVsAll, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );

	if ( !Q_stricmp( buffer, level.teamData.redName ) || !Q_stricmp( buffer, "red" ) || !Q_stricmp( buffer, "r" ) )
	{
	}
	else if ( !Q_stricmp( buffer, level.teamData.blueName ) || !Q_stricmp( buffer, "blue" ) || !Q_stricmp( buffer, "b" ) )
	{
	}
	else
	{
		G_PrintMessage( ent, "^1>>^7 Invalid team name.\n");
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: clanvsall \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", buffer );
	PrintCommand( type, "^7Clan vs All" );
	AdminCmd_DoClanVsAll( ent, buffer );
}

void AdminCmd_TagVsAll( gentity_t *ent, int type )
{
	char		buffer[MAX_TOKEN_CHARS];
	int			team1, team2;
	int			i;
	int			j;
	gclient_t	*cl;
	qboolean	found;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: tagvsall <team>\n");
		return;
	}

	if ( !level.gametypeData->teams ) {
		G_PrintMessage( ent, "^1>>^7 This is a non-team game.\n");
		return;
	}

	if ( !CheckCommand( &g_allowTagVsAll, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	if ( !Q_stricmp( buffer, level.teamData.redName ) || !Q_stricmp( buffer, "red" ) || !Q_stricmp( buffer, "r" ) ) 
	{
		team1 = TEAM_RED;
		team2 = TEAM_BLUE;
	} 
	else if ( !Q_stricmp( buffer, level.teamData.blueName ) || !Q_stricmp( buffer, "blue" ) || !Q_stricmp( buffer, "b" ) ) 
	{
		team1 = TEAM_BLUE;
		team2 = TEAM_RED;
	} 
	else 
	{
		G_PrintMessage( ent, "^1>>^7 Invalid team name.\n");
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: tagvsall \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", buffer );
	PrintCommand( type, "^7Tagged vs All" );

	for (i = 0; i < g_maxclients.integer; i++) 
	{
		cl = level.clients + i;
		if ( cl->pers.connected != CON_CONNECTED && cl->pers.connected != CON_CONNECTING ) 
		{
			continue;
		}
		if ( cl->sess.team == TEAM_FREE || cl->sess.team == TEAM_SPECTATOR ) 
		{
			continue;
		}

		found = qfalse;
		for (j = 0; j < level.tagcount; j++) 
		{
			if ( level.tags[ j ] == (cl - level.clients) ) {
				found = qtrue;
				break;
			}
		}
		if ( found ) {
			SetTeamInstant( &g_entities[i], team1, qfalse );
		}
		else {
			SetTeamInstant( &g_entities[i], team2, qfalse );
		}
	}
}

void AdminCmd_PbKick( gentity_t *ent, int type )
{
	char			buffer[MAX_TOKEN_CHARS];
	int				id, time;
	char			*text;
	gentity_t		*id_ent;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: pbkick #id #minutes <optional text>\n");
		return;
	}

	if ( !CheckCommand( &g_allowPbKick, ent, type, qtrue, qtrue, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	if ( !CheckID( ent, id ) ) {
		return;
	}

	id_ent = g_entities + id;
	trap_Argv( 3, buffer, sizeof( buffer ) );
	time = atoi( buffer );

	if ( !CheckRank( ent, id, qfalse ) ) {
		return;
	}

	if ( trap_Argc () > 4 ) {
		text = ConcatArgs( 4 );
	}
	else {
		text = va("kicked by admin for %d minutes", time);
	}

	G_PrintMessage( ent, va("^4>>^7 PunkBuster Kick request sent for player %s\n", id_ent->client->pers.netname) );
	G_AdminLogPrintf( "Admin (%i) action by %s: pbkick %i (%s) %i \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname, time, text );
	trap_SendConsoleCommand( EXEC_APPEND, va("pb_sv_kick \"%s\" %d %s\n", id_ent->client->pers.netname, time, text ) );
}

void AdminCmd_PbBan( gentity_t *ent, int type )
{
	char			buffer[MAX_TOKEN_CHARS];
	int				id;
	char			*text;
	gentity_t		*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: pbban #id <optional text>\n");
		return;
	}

	if ( !CheckCommand( &g_allowPbBan, ent, type, qtrue, qtrue, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	if ( !CheckID( ent, id ) ) {
		return;
	}

	id_ent = g_entities + id;

	if ( !CheckRank( ent, id, qfalse ) ) {
		return;
	}

	if ( trap_Argc () > 3 ) {
		text = ConcatArgs( 3 );
	}
	else {
		text = "banned by admin";
	}

	G_PrintMessage( ent, va("^4>>^7 PunkBuster Ban request sent for player %s\n", id_ent->client->pers.netname) );
	G_AdminLogPrintf( "Admin (%i) action by %s: pbban %i (%s) \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname, text );
	trap_SendConsoleCommand( EXEC_APPEND, va("pb_sv_ban \"%s\" %s\n", id_ent->client->pers.netname, text) );
}

void AdminCmd_PbGetSS( gentity_t *ent, int type )
{
	char			buffer[MAX_TOKEN_CHARS];
	int				id;
	gentity_t		*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: pbgetss #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowPbGetSS, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	if ( ent )
	{
		if ( id == -1 )
		{
			id = ent->client->sess.modData->pbssid;
		}
		else
		{
			ent->client->sess.modData->pbssid = id;
		}
	}

	if ( !CheckID( ent, id ) ) {
		return;
	}

	id_ent = g_entities + id;

	G_PrintMessage( ent, va("^4>>^7 PunkBuster Screenshot request sent for player %s\n", id_ent->client->pers.netname) );
	G_AdminLogPrintf( "Admin (%i) action by %s: pbgetss %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );
	trap_SendConsoleCommand( EXEC_APPEND, Q_CleanStr(va("pb_sv_getss \"%s\"\n", id_ent->client->pers.netname)) );
}

void AdminCmd_FreezeMap( gentity_t *ent, int type )
{
	if ( !CheckCommand( &g_allowFreezeMap, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	if ( level.freezeMap )
	{
		level.freezeMap = qfalse;
		G_AdminLogPrintf( "Admin (%i) action by %s: freezemap\n", type, ent?ent->client->pers.netname:"rcon" );
		PrintCommand( type, "^7Unfreeze mapcycle" );
	}
	else
	{
		level.freezeMap = qtrue;
		G_AdminLogPrintf( "Admin (%i) action by %s: freezemap\n", type, ent?ent->client->pers.netname:"rcon" );
		PrintCommand( type, "^7Freeze mapcycle" );
	}
}

void AdminCmd_DoStartMatch( void )
{
	level.match = qtrue;
	level.matchPending = 0;

	trap_Cvar_Set( "inMatch", g_autoMatchDemo.integer?"1":"2" );
//	inMatch.integer = 1;

	trap_SendConsoleCommand( EXEC_APPEND, "map_restart 0\n" );

	trap_SendServerCommand( -1, "cp \"^1Live on restart!\n\"");
}

void AdminCmd_StartMatch( gentity_t *ent, int type )
{
	if ( level.match ) {
		G_PrintMessage( ent, "^1>>^7 A match is already in progress.\n");
		return;
	}

	if ( !CheckCommand( &g_allowMatch, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: startmatch\n", type, ent?ent->client->pers.netname:"rcon" );
//	level.match = qtrue;
	level.matchPending = 1;
	trap_SendConsoleCommand( EXEC_APPEND, "exec \"cfgs/startmatch.cfg\"\n" );
	AdminCmd_DoPause("A match is about to begin");
}

void AdminCmd_EndMatch( gentity_t *ent, int type )
{
	if ( !level.match ) {
		G_PrintMessage( ent, "^1>>^7 No match is in progress.\n");
		return;
	}

	if ( !CheckCommand( &g_allowMatch, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: endmatch\n", type, ent?ent->client->pers.netname:"rcon" );
	level.match = qfalse;
	level.matchPending = 0;
	trap_Cvar_Set( "inMatch", "0" );
//	inMatch.integer = 0;
	if ( level.paused ) {
		level.unpauseTime = 0;
		level.unpauseCount = 0;
		AdminCmd_DoUnPause();
	}
	trap_SendConsoleCommand( EXEC_APPEND, "exec \"cfgs/endmatch.cfg\"\n" );
	PrintCommand( type, "^7Match has ended" );
}

void AdminCmd_ForceMatch( gentity_t *ent, int type )
{
	if ( !level.matchPending ) {
		G_PrintMessage( ent, "^1>>^7 A match is not pending.\n");
		return;
	}

	if ( !CheckCommand( &g_allowMatch, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: forcematch\n", type, ent?ent->client->pers.netname:"rcon" );
	level.matchPending = 2;
	level.unpauseTime = level.time;
}

void AdminCmd_Dummy( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	gentity_t	*id_ent;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: dummy #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowDummy, ent, type, qtrue, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		id_ent = g_entities + id;

		if ( !CheckRank( ent, id, qfalse ) ) {
			continue;
		}

		if ( g_entities[id].client->sess.modData->dummy ) {
			G_AdminLogPrintf( "Admin (%i) action by %s: undummy %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );
			g_entities[id].client->sess.modData->dummy = qfalse;
			recondata[id].attributes &= (~RECON_DUMMY);
			recondata[id].finishtime = level.time;
			ClientUserinfoChanged( id );
			trap_SendServerCommand( -1, va("print \"%s%s^7 is no longer a ^3TARGET DUMMY\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname));
		}
		else {
			G_AdminLogPrintf( "Admin (%i) action by %s: dummy %i (%s)\n", type, ent?ent->client->pers.netname:"rcon", id, id_ent->client->pers.netname );
			g_entities[id].client->sess.modData->dummy = qtrue;
			recondata[id].attributes |= RECON_DUMMY;
			recondata[id].finishtime = level.time;
			ClientUserinfoChanged( id );
			trap_SendServerCommand( -1, va("print \"%s%s^7 has been labeled a ^3TARGET DUMMY^7 by %s%s^7!\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
			if ( currtag == 0 ) {	// Only play the sound once
				G_BroadcastSound("sound/npc/air1/guard02/laughs.mp3");
			}
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_NoVote( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char			buffer[MAX_TOKEN_CHARS];
	int				id, time;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: novote #id #minutes\n");
		return;
	}

	if ( !CheckCommand( &g_allowNoVote, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		trap_Argv( 3, buffer, sizeof( buffer ) );
		time = atoi( buffer );

		if ( time == 0 )
		{
			time = 10;
		}

		if ( !CheckRank( ent, id, qfalse ) ) {
			continue;
		}

		G_AdminLogPrintf( "Admin (%i) action by %s: novote %i (%s) %i\n", type, ent?ent->client->pers.netname:"rcon", id, g_entities[id].client->pers.netname, time );
		recondata[id].voteDelayTime = level.time + (time * 60000);
		recondata[id].attributes |= RECON_NOVOTE;

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Rename( gentity_t *ent, int type )
{
	char			buffer[MAX_TOKEN_CHARS];
	char			userinfo[MAX_INFO_STRING];
	int				id;
	gentity_t		*id_ent;
	int				len;
	char			*text;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: rename #id <new name>\n");
		return;
	}

	if ( !CheckCommand( &g_allowRename, ent, type, qtrue, qtrue, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	if ( !CheckID( ent, id ) ) {
		return;
	}

	text = ConcatArgs( 3 );

	if ( strlen(text) > MAX_NETNAME ) {
		G_PrintMessage( ent, va("^1>>^7 Name is too long, more than %i characters.\n", MAX_NETNAME) );
		return;
	}

	id_ent = g_entities + id;

	if ( !CheckRank( ent, id, qfalse ) ) {
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: rename %i (%s) \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", id, g_entities[id].client->pers.netname, text );
	trap_GetUserinfo( id, userinfo, sizeof( userinfo ) );
	Info_SetValueForKey( userinfo, "name", text );
	trap_SetUserinfo( id, userinfo );
	ClientUserinfoChanged( id );
}

void AdminCmd_CvarCheck( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char		buffer[MAX_TOKEN_CHARS];
	int			id;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: cvarcheck #id <cvar>\n");
		return;
	}

	if ( !CheckCommand( &g_allowCvarCheck, ent, type, qtrue, qtrue, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		trap_Argv( 3, buffer, sizeof( buffer ) );

		G_PrintMessage( ent, va("^4>>^7 Cvar request sent to %s\n", g_entities[id].client->pers.netname) );
		trap_SendServerCommand( id, va("cvarcheck %i %s", ent?ent->s.number:999, buffer) );

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

/*	// Disabled for now, too dangerous
void AdminCmd_CvarSet( gentity_t *ent, int type )
{
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	char		*text;

	if ( trap_Argc () < 5 ) {
		G_PrintMessage( ent, "usage: cvarset #id <cvar> <value>\n");
		return;
	}

	if ( !CheckCommand( &g_allowCvarCheck, ent, type, qfalse, qfalse, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	if ( !CheckID( ent, id ) ) {
		return;
	}

	trap_Argv( 3, buffer, sizeof( buffer ) );
	text = ConcatArgs( 4 );

	G_PrintMessage( ent, va("^4>>^7 Cvar change sent to %s\n", g_entities[id].client->pers.netname) );
	trap_SendServerCommand( id, va("cvarset %s %s", buffer, text) );
}
*/

void AdminCmd_Cfg( gentity_t *ent, int type )
{
	char			buffer[MAX_TOKEN_CHARS];
	fileHandle_t	f;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: cfg <filename>\n");
		return;
	}

	if ( !CheckCommand( &g_allowCfg, ent, type, qtrue, qtrue, qfalse ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	trap_FS_FOpenFile( va("cfgs/%s.cfg", buffer), &f, FS_READ_TEXT );
	if ( !f ) {
		G_PrintMessage( ent, va("^1>>^7 Config file ^5%s.cfg^7 doesn't exist\n", buffer) );
		return;
	}
	trap_FS_FCloseFile( f );
	G_PrintMessage( ent, va("^4>>^7 Executing config file ^5%s.cfg\n", buffer) );
	G_AdminLogPrintf( "Admin (%i) action by %s: cfg \"%s.cfg\"\n", type, ent?ent->client->pers.netname:"rcon", buffer );
	trap_SendConsoleCommand( EXEC_APPEND, va("exec \"cfgs/%s\"\n", buffer) );
}

void AdminCmd_Tag( gentity_t *ent, int type )
{
	char		buffer[MAX_TOKEN_CHARS];
	int			id;
	int			i;
	int			j;
	char		*text;
	gentity_t	*player;
	qboolean	found;
	qboolean	all;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: tag <id or string>\n");
		return;
	}

	if ( !CheckCommand( &g_allowTag, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	if ( buffer[0] == '#' || (buffer[0] >= '1' && buffer[0] <= '9') ) {
		id = ParseID( ent, buffer );

		if ( !CheckID( ent, id ) ) {
			return;
		}
		found = qfalse;
		for (j = 0; j < level.tagcount; j++) 
		{
			if ( level.tags[ j ] == id ) {
				found = qtrue;
				G_PrintMessage( ent, "^1>>^7 Player is already tagged.\n");
				break;
			}
		}
		if ( !found ) {
			G_PrintMessage( ent, va("^4>>^7 Tagging player %s\n", g_entities[id].client->pers.netname) );
			level.tags[ level.tagcount++ ] = id;
		}
	}
	else {
		text = ConcatArgs( 2 );

		all = qfalse;
		if ( Q_stricmp (text, "all") == 0 )
		{
			all = qtrue;
		}
		for (i = 0; i < level.numConnectedClients; i++) 
		{
			player = &g_entities[level.sortedClients[i]];
			if ( all || strstr( Q_CleanStr(va("%s",player->client->pers.netname)), text ) ) {
				found = qfalse;
				for (j = 0; j < level.tagcount; j++) 
				{
					if ( level.tags[ j ] == level.sortedClients[i] ) {
						found = qtrue;
						break;
					}
				}
				if ( !found ) {
					G_PrintMessage( ent, va("^4>>^7 Tagging player %s\n", g_entities[level.sortedClients[i]].client->pers.netname) );
					level.tags[ level.tagcount++ ] = level.sortedClients[i];
				}
			}
		}
	}

	G_PrintMessage( ent, va("^4>>^7 There are now ^5%i^7 players tagged\n", level.tagcount) );
	G_AdminLogPrintf( "Admin (%i) action by %s: tag \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", buffer );
}

void AdminCmd_SwapTags( gentity_t *ent, int type )
{
	int			i;
	int			j;
	qboolean	found;
	char		tags[64];			// tag a maximum of 64 players
	int			tagcount;


	if ( !CheckCommand( &g_allowTag, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	tagcount = 0;
	for (i = 0; i < level.numConnectedClients; i++) 
	{
		found = qfalse;
		for (j = 0; j < level.tagcount; j++) 
		{
			if ( level.tags[ j ] == level.sortedClients[i] ) {
				found = qtrue;
				break;
			}
		}
		if ( !found ) {
			tags[ tagcount++ ] = level.sortedClients[i];
		}
	}

	level.tagcount = tagcount;
	memcpy( level.tags, tags, 64 );

	G_PrintMessage( ent, va("^4>>^7 There are now ^5%i^7 players tagged\n", level.tagcount) );
	G_AdminLogPrintf( "Admin (%i) action by %s: swaptags\n", type, ent?ent->client->pers.netname:"rcon" );
}

void AdminCmd_ClearTags( gentity_t *ent, int type )
{
	if ( !CheckCommand( &g_allowTag, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	level.tagcount = 0;

	G_PrintMessage( ent, va("^4>>^7 There are now ^5%i^7 players tagged\n", level.tagcount) );
	G_AdminLogPrintf( "Admin (%i) action by %s: cleartags\n", type, ent?ent->client->pers.netname:"rcon" );
}

void AdminCmd_Cripple( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char			buffer[MAX_TOKEN_CHARS];
	int				id, value;

	if ( trap_Argc () < 4 ) {
		G_PrintMessage( ent, "usage: cripple #id #percentage\n");
		return;
	}

	if ( !CheckCommand( &g_allowCripple, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		trap_Argv( 3, buffer, sizeof( buffer ) );
		value = atoi( buffer );

		if ( value > 100 )
		{
			value = 100;
		}
		if ( value < 0 )
		{
			value = 0;
		}

		if ( !CheckRank( ent, id, qfalse ) ) {
			continue;
		}

		trap_SendServerCommand( -1, va("print \"%s%s^7 is now %i%% crippled by %s%s\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, value, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
		G_AdminLogPrintf( "Admin (%i) action by %s: cripple %i (%s) %i\n", type, ent?ent->client->pers.netname:"rcon", id, g_entities[id].client->pers.netname, value );
		recondata[id].crippleLevel = value;

		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/npc/col2/anderson/04augh.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Backfire( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char			buffer[MAX_TOKEN_CHARS];
	int				id;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: backfire #id\n");
		return;
	}

	if ( !CheckCommand( &g_allowBackfire, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	trap_Argv( 2, buffer, sizeof( buffer ) );
	id = ParseID( ent, buffer );

	currtag = 0;											//
	usetags = CheckTag( ent, id );							//
	do {													//
		if ( usetags ) {									//
			id = level.tags[ currtag ];						//
		}													//

		if ( !CheckID( ent, id ) ) {
			continue;
		}

		if ( !CheckRank( ent, id, qfalse ) ) {
			continue;
		}

		if ( recondata[id].attributes & RECON_BACKFIRE ) {
			trap_SendServerCommand( -1, va("print \"%s%s^7 is no longer backfiring with the help of %s%s\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
			G_AdminLogPrintf( "Admin (%i) action by %s: backfire %i (%s) off\n", type, ent?ent->client->pers.netname:"rcon", id, g_entities[id].client->pers.netname );
			recondata[id].attributes &= ~RECON_BACKFIRE;
		}
		else {
			trap_SendServerCommand( -1, va("print \"%s%s^7 is now backfiring with the help of %s%s\n\"", level.teamData.teamcolor[g_entities[id].client->sess.team], g_entities[id].client->pers.netname, (!ent||(type==1&&g_anonymousAdmin.integer))?"":level.teamData.teamcolor[ent->client->sess.team], (!ent||(type==1&&g_anonymousAdmin.integer))?"an admin":ent->client->pers.netname));
			G_AdminLogPrintf( "Admin (%i) action by %s: backfire %i (%s) on\n", type, ent?ent->client->pers.netname:"rcon", id, g_entities[id].client->pers.netname );
			recondata[id].attributes |= RECON_BACKFIRE;
		}

		if ( currtag == 0 ) {	// Only play the sound once
			G_BroadcastSound("sound/npc/civ/english/male/cantbe.mp3");
		}

	} while ( usetags && ++currtag < level.tagcount );		//
	if ( usetags == 2 ) {									//
		level.tagcount = 0;									//
	}														//
}

void AdminCmd_Password( gentity_t *ent, int type )
{
	int				usetags;								//
	int				currtag;								//
	char			buffer[MAX_TOKEN_CHARS];
	char			*text;

	if ( trap_Argc () < 3 ) {
		G_PrintMessage( ent, "usage: password <password>\n");
		return;
	}

	if ( !CheckCommand( &g_allowPassword, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	text = ConcatArgs( 2 );

	G_AdminLogPrintf( "Admin (%i) action by %s: password \"%s\"\n", type, ent?ent->client->pers.netname:"rcon", text );
	trap_SendConsoleCommand( EXEC_APPEND, va("g_password %s\n", text) );
}

void AdminCmd_Reset( gentity_t *ent, int type )
{
	if ( !CheckCommand( &g_allowReset, ent, type, qtrue, qtrue, qtrue ) )
	{
		return;
	}

	G_AdminLogPrintf( "Admin (%i) action by %s: reset\n", type, ent?ent->client->pers.netname:"rcon" );
	G_ResetGametypeEntities();
}

