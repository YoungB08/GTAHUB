/*
    Written by s4marium

   todo:
        - Add admin command to set player XP/level (e.g. /setxp [player] [amount], /setlevel [player] [level])
        - Add more XP gain sources (e.g. completing missions, killing NPCs, etc.)
        - Add level up rewards (e.g. give player money, items, etc. on level up)
        - Add XP gain multiplier for premium players (e.g. 2x XP gain for premium members)
        - Optimize database queries by batching XP saves instead of saving on every changeeach
        - Add XP requirements display in class selection menu to show how much XP is needed for class or job unlocks
        - Add leaderboard to show top players by level or XP
        - Add achievements related to XP and levels (e.g. reach level 10, gain 1000 XP, etc.)
*/
#include <YSI_Coding\y_hooks>

// =============================================================================
// DEFINITIONS & CONSTANTS
// =============================================================================

#define MAX_XP_PER_LEVEL(%0) ((%0 + 3) * 150)
#define XP_HIDE_TIMER 6000

// =============================================================================
// ENUMS
// =============================================================================

enum E_PLAYER_XP_DATA
{
	pXp,
	pLevel
};

// =============================================================================
// STATIC VARIABLES
// =============================================================================

static
	PlayerXpData[MAX_PLAYERS][E_PLAYER_XP_DATA],
	Statement:stmt_savePlayerXp,
	Statement:stmt_loadPlayerXp,
	PlayerXpUpdateTimer[MAX_PLAYERS];

// =============================================================================
// HOOKS - INITIALIZATION
// =============================================================================

hook OnMySQLConnected() {
	stmt_savePlayerXp = MySQL_PrepareStatement(MySQL_GetHandle(),
		"UPDATE player_stats SET xp = ?, level = ? WHERE u_id = ?");

	stmt_loadPlayerXp = MySQL_PrepareStatement(MySQL_GetHandle(),
		"SELECT xp, level FROM player_stats WHERE u_id = ?");

	return 1;
}

hook OnPlayerConnect(playerid) {
	PlayerXpData[playerid][pXp] = 0;
	PlayerXpData[playerid][pLevel] = 0;

	if(PlayerXpUpdateTimer[playerid] != -1) {
		KillTimer(PlayerXpUpdateTimer[playerid]);
		PlayerXpUpdateTimer[playerid] = -1;
	}

	return 1;
}

hook OnPlayerLogin(playerid) {
	XpSystem_Load(playerid);
	return 1;
}

hook OnPlayerAutoSave(playerid) {
	XpSystem_Save(playerid);
	return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
	if(PlayerXpUpdateTimer[playerid] != -1) {
		KillTimer(PlayerXpUpdateTimer[playerid]);
		PlayerXpUpdateTimer[playerid] = -1;
	}
	return 1;
}

// =============================================================================
// INTERNAL FUNCTIONS
// =============================================================================

XpSystem_Load(playerid) {
	inline OnXpLoad() {
		new
			xp = 0,
			level = 0;

		MySQL_BindResultInt(stmt_loadPlayerXp, 0, xp);
		MySQL_BindResultInt(stmt_loadPlayerXp, 1, level);

		if(MySQL_Statement_FetchRow(stmt_loadPlayerXp)) {
			PlayerXpData[playerid][pXp] = xp;
			PlayerXpData[playerid][pLevel] = level;
		} else {
			// First time: initialize with 0 XP, level 1
			PlayerXpData[playerid][pXp] = 0;
			PlayerXpData[playerid][pLevel] = 1;
		}
	}

	MySQL_BindInt(stmt_loadPlayerXp, 0, Player_GetAccountID(playerid));
	MySQL_ExecuteParallel_Inline(stmt_loadPlayerXp, using inline OnXpLoad);
}

XpSystem_Save(playerid) {
	MySQL_BindInt(stmt_savePlayerXp, 0, PlayerXpData[playerid][pXp]);
	MySQL_BindInt(stmt_savePlayerXp, 1, PlayerXpData[playerid][pLevel]);
	MySQL_BindInt(stmt_savePlayerXp, 2, Player_GetAccountID(playerid));
	MySQL_ExecuteThreaded(stmt_savePlayerXp);
}

forward XpUI_ShowGain(playerid, amount);

// =============================================================================
// PUBLIC API FUNCTIONS
// =============================================================================

/**
 * Give XP to player and handle level up
 *
 * @param playerid Player ID
 * @param amount XP amount to give
 * @return true on success
 */
stock bool:GivePlayerXp(playerid, amount) {
	if(amount <= 0 || !IsPlayerConnected(playerid)) {
		return false;
	}

	PlayerXpData[playerid][pXp] += amount;

	// Show XP gain notification
	XpUI_ShowGain(playerid, amount);

	// Check for level up
	while(PlayerXpData[playerid][pXp] >= MAX_XP_PER_LEVEL(PlayerXpData[playerid][pLevel])) {
		PlayerXpData[playerid][pXp] -= MAX_XP_PER_LEVEL(PlayerXpData[playerid][pLevel]);
		PlayerXpData[playerid][pLevel]++;

		// Level up notification
		SendClientMessage(playerid, COLOR_RED, "Chuc mung!"C_WHITE" Ban da len level!");
		PlayerPlaySound(playerid, 17803, 0.0, 0.0, 0.0);

		// Update score to match level
		SetPlayerScore(playerid, PlayerXpData[playerid][pLevel]);
	}

	// Update score
	SetPlayerScore(playerid, PlayerXpData[playerid][pLevel]);

	// Save to database
	XpSystem_Save(playerid);

	return true;
}

/**
 * Remove XP from player
 *
 * @param playerid Player ID
 * @param amount XP amount to remove
 * @return true on success
 */
stock bool:RemovePlayerXp(playerid, amount) {
	if(amount <= 0 || !IsPlayerConnected(playerid)) {
		return false;
	}

	PlayerXpData[playerid][pXp] = Max(0, PlayerXpData[playerid][pXp] - amount);
	return true;
}

/**
 * Get player's current XP
 *
 * @param playerid Player ID
 * @return Current XP amount
 */
stock GetPlayerXp(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return 0;
	}
	return PlayerXpData[playerid][pXp];
}

/**
 * Get player's current level
 *
 * @param playerid Player ID
 * @return Current level
 */
stock GetPlayerLevel(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return 0;
	}
	return PlayerXpData[playerid][pLevel];
}

/**
 * Set player's XP to specific value
 *
 * @param playerid Player ID
 * @param xp XP amount
 * @return true on success
 */
stock bool:SetPlayerXp(playerid, xp) {
	if(xp < 0 || !IsPlayerConnected(playerid)) {
		return false;
	}
	PlayerXpData[playerid][pXp] = xp;
	return true;
}

/**
 * Set player's level to specific value
 *
 * @param playerid Player ID
 * @param level Level to set
 * @return true on success
 */
stock bool:SetPlayerLevel(playerid, level) {
	if(level < 1 || !IsPlayerConnected(playerid)) {
		return false;
	}
	PlayerXpData[playerid][pLevel] = level;
	SetPlayerScore(playerid, level);
	return true;
}

/**
 * Get XP required for next level
 *
 * @param playerid Player ID
 * @return XP required for next level
 */
stock GetPlayerXpRequired(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return 0;
	}
	return MAX_XP_PER_LEVEL(PlayerXpData[playerid][pLevel]);
}

/**
 * Get player's progress to next level (0-100)
 *
 * @param playerid Player ID
 * @return Progress percentage
 */
stock GetPlayerXpProgress(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return 0;
	}

	new xpRequired = MAX_XP_PER_LEVEL(PlayerXpData[playerid][pLevel]);
	return (PlayerXpData[playerid][pXp] * 100) / xpRequired;
}

/**
 * Reset player's XP and level
 *
 * @param playerid Player ID
 * @return true on success
 */
stock bool:ResetPlayerXp(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return false;
	}
	PlayerXpData[playerid][pXp] = 0;
	PlayerXpData[playerid][pLevel] = 1;
	SetPlayerScore(playerid, 1);
	return true;
}

// =============================================================================
// TEST COMMAND
// =============================================================================
CMD:getlevel(playerid, params[]) {
	GivePlayerXp(playerid, 100);
	new message[128];
	format(message, sizeof(message), "Level: %d | XP: %d/%d",
		GetPlayerLevel(playerid),
		GetPlayerXp(playerid),
		GetPlayerXpRequired(playerid));
	SendClientMessage(playerid, -1, message);
	return 1;
}
