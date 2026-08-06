// Patrick Dave Subang (c) 2020 April
// Github -> https://github.com/PatrickGTR

// Ervin Lance Zamora (c) 2020 April
// Github -> https://github.com/CnRXoMoX

#define _INC_omp_npc

// Credits to these people, made the production easier.
// Southclaw, Y_Less, maddinatOr, SyS, Zeex, Slice, Lorenc

// TextdrawLetterSize Rule -> Thanks to DamianC
// Letter-size-y = letter-size-x * 4
// For a nice font display
#define SAMP_COMPAT
#define CHAT_MAX_CHARACTERS    (144)
#define CHAT_MAX_UTF8_BYTES    (768)
#define CHAT_FORMATTED_BYTES   (896)


#if 0
	// called when player successfully logged in.
	forward OnPlayerLogin(playerid);

	// called when player successfully registered.
	forward OnPlayerRegister(playerid);

	// called when player's ip/name/gcpi wasn't found in the ban database.
	forward OnPlayerPassedBanCheck(playerid);

	// called every second per player (like OnPlayerUpdate but not as intensive.)
	forward OnPlayerSecondUpdate(playerid);

	// called when player spawns for the first time after initial connection.
	forward OnPlayerFirstSpawn(playerid);

	// called every 2 minutes for datas that need to be saved frequently.
	forward OnPlayerAutoSave(playerid);

	// called when player has robbed another player.
	forward OnPlayerRobPlayer(playerid, targetid, moneyTaken);

	// called when player has successfully robbed a store.
	forward OnPlayerRobStore(playerid, moneyTaken);

	// called when c4 has been detonated.
	forward OnExplosiveDetonate(playerid, Float: x, Float: y, Float: z);

	// called every second.
	forward OnServerSecondUpdate();

	// called when MySQL successfully connects. (mainly used to setup tables & prepare statements)
	forward OnMySQLConnected();

	// called before MySQL disconnects (before mysql_close specifically)
	forward OnMySQLPreClose();

	// called when the in-game week resets.
	forward OnServerWeekReset();

	// called when the in-game day resets.
	forward OnServerDayReset(const day[]);
#endif


// Custom Functions

#if 0
	// playerid -> player to give score.
	// score -> amount of score to give.
	// save -> false by default, toggle to allow saving.
	Player_GiveScore(playerid, score, bool:save = false);

	// playerid -> player to remove score.
	// score -> how much you want to remove from the player.
	Player_RemoveScore(playerid, score);
#endif

// Main
#include <constants>

// Libraries
#include <a_mysql>
#include <samp_bcrypt>
#include <cef>
#include <PawnPlus>

// YSI
#include <YSI_Core\y_utils>
#include <YSI_Coding\y_inline>
#include <YSI_Coding\y_timers>
#include <YSI_Data\y_bit>
#include <YSI_Data\y_iterate>

#include <YSI_Extra\y_inline_mysql>
#include <YSI_Extra\y_inline_bcrypt>

// Legacy Includes
#include <EVF>
#include <progress2>


#include <ini>

// #include <env>
#include <streamer>
#include <logger>
#include <mysql_prepared>

stock MySQL_BindString(Statement:statement, param, const value[], len = sizeof(value))
{
	#pragma unused len
	MySQL_Bind(statement, param, value);
	return 1;
}

stock MySQL_BindResultString(Statement:statement, field, const value[], len = sizeof(value))
{
	MySQL_BindResult(statement, field, value, len);
	return 1;
}

#include <map-zones>
//#include <formatex>

// MySQL Config
#define MYSQL_HOST      "127.0.0.1"
#define MYSQL_USER      "root"
#define MYSQL_PASSWORD  "123456"
#define MYSQL_DATABASE  "gtahub"

// Gamemode Scripts



#include <init>

#if SETUP_TABLE
	#include <tables>
#endif

#include <utils>
#include <user-interface>
#include <interaction>
#include <anti-cheat> // w.i.p
#include <server>

// CEF Core System
#include <cef_core>

#include <account>
#include <player>
#include <houses>
#include <admin>
#include <system>
#include <chat> // chat & messaging
// HUB-Core Role Component Include
#include <hubcore_role>

#include <cmds>
#include <mapping>

// CEF Logic Modules
#include <cef_login>
#include <cef_register>
#include <cef_characters>

#include <gangs>

// Will be called after the rest ^
public OnGameModeInit() {
	Message_SetTime(5);
    Message_Add("Chao mung den voi GTAHUB");
    Message_Add("Neu thich GTAHUB, hay them server vao danh sach yeu thich!");
	Message_Add("Website chinh thuc: GTAHUB.vn");
    Message_Add("Ung ho de giup server phat trien lau dai!");

	// Role Component Config
	SetRoleGlobalConfig(25.0, true, true, 32.0, 32.0);


	SendRconCommand("name "#SERVER_NAME " v" #SCRIPT_VERSION_MAJOR "." #SCRIPT_VERSION_MINOR "." #SCRIPT_VERSION_PATCH);
	SendRconCommand("game.mode "SERVER_MODE"");
	SendRconCommand("language "SERVER_LANGUAGE"");
	SendRconCommand("website "SERVER_WEBSITE"");

	SetWorldTime(23);
	SetWeather(0);
	DisableInteriorEnterExits();
	EnableStuntBonusForAll(false);
	UsePlayerPedAnims();

	// Init Vehicles
	return 1;
}

public OnPlayerDeathEx(playerid, killerid, reason) {
	if(IsPlayerConnected(killerid)) {
		Player_GiveKill(killerid, 1, true);
		SendDeathMessage(killerid, playerid, reason);
	}

	Player_SetDeaths(playerid, 1, true);
	SendDeathMessage(INVALID_PLAYER_ID, playerid, reason);
	return 1;
}

// temporary fix for players not taking damage, although api should handle this when
// not in use.
public OnPlayerDamagePlayer(playerid, issuerid, &Float: amount, weaponid, bodypart)
{
	if(Player_GetClass(playerid) == Player_GetClass(issuerid) && Player_GetClass(playerid) != TEAM_CIVILIAN) {
		return 0; // no team damage.
	}

    return 1; // returning 0 will prevent user from taking damage (THIS IS A BIG FEATURE!)
}

// TEMP - MUST REMOVE!
CMD:kill(playerid, params[]) {
	SetPlayerHealth(playerid, 0.0);
	return 1;
}

CMD:killveh(playerid, params) {
	SetVehicleHealth(GetPlayerVehicleID(playerid), 0.0);
	return 1;
}
