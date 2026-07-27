/*
    Written by s4marium
*/
#include <YSI_Coding\y_hooks>

// =============================================================================
// DEFINITIONS & CONSTANTS
// =============================================================================

#define MAX_XP_PER_LEVEL(%0) ((%0 + 3) * 150)
#define XP_HIDE_TIMER 6000
#define XP_SAVE_DELAY 30000
#define XP_LEVEL_REWARD(%0) ((%0) * 1000)

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
	PlayerXpUpdateTimer[playerid] = -1;

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
	if(PlayerXpData[playerid][pLevel] > 0) {
		XpSystem_Save(playerid);
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
	if(!IsPlayerConnected(playerid) || Player_GetAccountID(playerid) <= 0) {
		return 0;
	}

	MySQL_BindInt(stmt_savePlayerXp, 0, PlayerXpData[playerid][pXp]);
	MySQL_BindInt(stmt_savePlayerXp, 1, PlayerXpData[playerid][pLevel]);
	MySQL_BindInt(stmt_savePlayerXp, 2, Player_GetAccountID(playerid));
	MySQL_ExecuteThreaded(stmt_savePlayerXp);
	return 1;
}

static XpSystem_QueueSave(playerid) {
	if(!IsPlayerConnected(playerid) || Player_GetAccountID(playerid) <= 0) {
		return 0;
	}

	if(PlayerXpUpdateTimer[playerid] == -1) {
		PlayerXpUpdateTimer[playerid] = SetTimerEx("XpSystem_SaveTimer", XP_SAVE_DELAY, false, "i", playerid);
	}
	return 1;
}

static XpSystem_ApplyMultiplier(playerid, amount) {
	new
		vipLevel = VIP_GetPlayerLevel(playerid),
		bonusPercent = min(vipLevel * 10, 50);

	return amount + ((amount * bonusPercent) / 100);
}

forward XpUI_ShowGain(playerid, amount);
forward XpUI_ShowLevelUp(playerid, level);
forward XpSystem_SaveTimer(playerid);

public XpSystem_SaveTimer(playerid) {
	PlayerXpUpdateTimer[playerid] = -1;

	if(IsPlayerConnected(playerid)) {
		XpSystem_Save(playerid);
	}
	return 1;
}

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

	amount = XpSystem_ApplyMultiplier(playerid, amount);
	PlayerXpData[playerid][pXp] += amount;

	// Show XP gain notification
	XpUI_ShowGain(playerid, amount);

	// Check for level up
	while(PlayerXpData[playerid][pXp] >= MAX_XP_PER_LEVEL(PlayerXpData[playerid][pLevel])) {
		PlayerXpData[playerid][pXp] -= MAX_XP_PER_LEVEL(PlayerXpData[playerid][pLevel]);
		PlayerXpData[playerid][pLevel]++;

		new levelReward = XP_LEVEL_REWARD(PlayerXpData[playerid][pLevel]);
		GivePlayerMoney(playerid, levelReward);

		SendClientMessage(playerid, COLOR_RED, "Chuc mung!"C_WHITE" Ban da len level!");
		SendServerMsgF(playerid, "Thuong len level: "C_GREEN"%m"C_WHITE".", levelReward);
		XpUI_ShowLevelUp(playerid, PlayerXpData[playerid][pLevel]);

		// Update score to match level
		SetPlayerScore(playerid, PlayerXpData[playerid][pLevel]);
	}

	// Update score
	SetPlayerScore(playerid, PlayerXpData[playerid][pLevel]);

	XpSystem_QueueSave(playerid);

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
	XpSystem_QueueSave(playerid);
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
	XpSystem_QueueSave(playerid);
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
	XpSystem_QueueSave(playerid);
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
	XpSystem_QueueSave(playerid);
	return true;
}

// =============================================================================
// TEST COMMAND
// =============================================================================
CMD:getlevel(playerid, params[]) {
	new message[128];
	format(message, sizeof(message), "Level: %d | XP: %d/%d",
		GetPlayerLevel(playerid),
		GetPlayerXp(playerid),
		GetPlayerXpRequired(playerid));
	SendClientMessage(playerid, -1, message);
	return 1;
}

CMD:xpleaderboard(playerid, params[]) {
	#pragma unused params

	inline const OnXpLeaderboardLoad() {
		new
			rows = cache_num_rows(),
			name[MAX_PLAYER_NAME],
			level,
			xp,
			line[96],
			content[1024];

		strcat(content, "Hang\tNguoi choi\tLevel\tXP\n", sizeof(content));

		for(new i = 0; i < rows; i++) {
			cache_get_value_name(i, "username", name, sizeof(name));
			cache_get_value_name_int(i, "level", level);
			cache_get_value_name_int(i, "xp", xp);

			format(line, sizeof(line), "%d\t%s\t%d\t%d/%d\n", i + 1, name, level, xp, MAX_XP_PER_LEVEL(level));
			strcat(content, line, sizeof(content));
		}

		Dialog_Show(playerid, DIALOG_STYLE_TABLIST_HEADERS, "Bang Xep Hang XP", content, "Dong", "");
		return 1;
	}

	MySQL_TQueryInline(MySQL_GetHandle(), using inline OnXpLeaderboardLoad, "\
		SELECT \
			CONCAT(p.firstname, ' ', p.lastname) AS username, ps.level, ps.xp \
		FROM \
			player_stats AS ps \
		INNER JOIN \
			players AS p \
		ON \
			p.u_id = ps.u_id \
		WHERE \
			p.is_character = 1 \
		ORDER BY \
			ps.level DESC, ps.xp DESC \
		LIMIT 10");
	return 1;
}

CMD:setxp(playerid, params[]) {
	new
		targetid,
		amount;

	if(Admin_GetPlayerLevel(playerid) < 5) {
		return SendErrorMsg(playerid, "Ban khong co quyen dung lenh nay.");
	}

	if(sscanf(params, "ui", targetid, amount)) {
		return SendSyntaxMsg(playerid, "/setxp <playerid> <xp>");
	}

	if(!SetPlayerXp(targetid, amount)) {
		return SendErrorMsg(playerid, "Nguoi choi hoac so XP khong hop le.");
	}

	SendMsgF(playerid, COLOR_MODERATION, "[ADMIN XP]: Da dat XP cua %p(%d) thanh %d.", targetid, targetid, amount);
	SendMsgF(targetid, COLOR_MODERATION, "[ADMIN XP]: XP cua ban da duoc dat thanh %d.", amount);
	return 1;
}

CMD:setlevel(playerid, params[]) {
	new
		targetid,
		level;

	if(Admin_GetPlayerLevel(playerid) < 5) {
		return SendErrorMsg(playerid, "Ban khong co quyen dung lenh nay.");
	}

	if(sscanf(params, "ui", targetid, level)) {
		return SendSyntaxMsg(playerid, "/setlevel <playerid> <level>");
	}

	if(!SetPlayerLevel(targetid, level)) {
		return SendErrorMsg(playerid, "Nguoi choi hoac level khong hop le.");
	}

	SendMsgF(playerid, COLOR_MODERATION, "[ADMIN XP]: Da dat level cua %p(%d) thanh %d.", targetid, targetid, level);
	SendMsgF(targetid, COLOR_MODERATION, "[ADMIN XP]: Level cua ban da duoc dat thanh %d.", level);
	return 1;
}
