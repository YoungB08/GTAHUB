#include <YSI_Coding\y_hooks>

static
    gShotNearPoliceTick[MAX_PLAYERS],
    gShotPoliceTick[MAX_PLAYERS],
    gEnteredPoliceVehicleTick[MAX_PLAYERS];

hook OnPlayerConnect(playerid) {
    gShotNearPoliceTick[playerid] = 0;
    gShotPoliceTick[playerid] = 0;
    gEnteredPoliceVehicleTick[playerid] = 0;
    return 1;
}

// Entering Police Vehicle
hook OnPlayerStateChange(playerid, newstate, oldstate) {
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    if(newstate != PLAYER_STATE_DRIVER) {
        return 1;
    }

    if(Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return 1;
    }

    if((gettime() - gEnteredPoliceVehicleTick[playerid]) < (60 * 3)) {
        return 1;
    }

    new
        vehicleid = GetPlayerVehicleID(playerid);

    if(!IsVehiclePolice(vehicleid)) {
        return 1;
    }

    Player_GiveWantedLevel(playerid, 4);
    SendMsgF(playerid, COLOR_RED, "Ban da vao xe canh sat, ban bi cong them 4 sao truy na.");
    gEnteredPoliceVehicleTick[playerid] = gettime();
    return 1;
}

// Murder
hook OnPlayerDeathEx(playerid, killerid, reason) {
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    if(killerid != INVALID_PLAYER_ID) {
        Player_GiveWantedLevel(killerid, 6);
        SendMsgF(killerid, COLOR_RED, "Ban da giet nguoi, ban bi cong them 6 sao truy na.");
    }
    return 1;
}

// Store Robbery
hook OnPlayerRobStore(playerid, moneyTaken) {
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    new
        location[MAX_MAP_ZONE_NAME];
    Player_GetLocation(playerid, location, sizeof(location));
    SendMsgAllF(COLOR_ORANGE, "(cuop cua hang) "C_GREY"%p(%i) "C_WHITE"da lay "C_GREEN"%m "C_WHITE"tu "C_YELLOW"%s "C_WHITE"tai "C_GREY"%s.", playerid, playerid, moneyTaken, Entrance_PlayerIn(playerid), location);

    GivePlayerMoney(playerid, moneyTaken);
    Player_GiveWantedLevel(playerid, 4);
    return 1;
}

// Player Roberry
hook OnPlayerRobPlayer(playerid, targetid, moneyTaken) {
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    SendMsgF(playerid, COLOR_ORANGE, "(cuop nguoi choi) "C_WHITE"Ban da cuop thanh cong "C_GREY"%p (%i) "C_WHITE"va tim thay "C_GREEN"%m "C_WHITE"trong vi.", targetid, targetid, moneyTaken);
    SendMsgF(targetid, COLOR_ORANGE, "(cuop nguoi choi) "C_WHITE"Ban da bi "C_GREY"%p (%i) "C_WHITE"cuop va mat "C_GREEN"%m "C_WHITE"trong vi.", playerid, playerid, moneyTaken);

    GivePlayerMoney(targetid, -moneyTaken);
    GivePlayerMoney(playerid, moneyTaken);
    Player_GiveWantedLevel(playerid, 4);

    return 1;
}

// Shooting near a cop
hook OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ) {
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    // shooter is not civilian.
    if(Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return 1;
    }

    // ignore if player is wanted already.
    if(GetPlayerWantedLevel(playerid) >= 4) {
        return 1;
    }

    // disallow spam, only increase wanted level after 3 minutes.
    if((gettime() - gShotNearPoliceTick[playerid]) < (60 * 3)) {
        return 1;
    }

    new
        Float:distance = 20.0,
        closestID = Player_GetClosestLeo(playerid, distance);


    // no closest law enforce officers
    if(closestID == INVALID_PLAYER_ID) {
        return 1;
    }

    SendMsgF(playerid, COLOR_RED, "Ban da ban gan canh sat dang lam nhiem vu, ban bi cong them 4 sao truy na.");
    Player_GiveWantedLevel(playerid, 4);
    gShotNearPoliceTick[playerid] = gettime();
    return 1;
}

// Shooting a cop detection.
hook OnPlayerDmgPlayerDone(playerid, issuerid, &Float: amount, weaponid, bodypart ) {
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    // shooter is not civilian.
    if(Player_GetClass(issuerid) != TEAM_CIVILIAN) {
        return 1;
    }

    // ignore if player is wanted already.
    if(GetPlayerWantedLevel(issuerid) >= 4) {
        return 1;
    }

    // disallow spam, only issue this crime every 3 minutes.
    if((gettime() - gShotPoliceTick[issuerid]) < (60 * 3)) {
        return 1;
    }

    // victim is not police.
    if(Player_GetClass(playerid) != TEAM_POLICE) {
        return 1;
    }

    SendMsgF(issuerid, COLOR_RED, "Ban da ban canh sat dang lam nhiem vu, ban bi cong them 4 sao truy na.");
    Player_GiveWantedLevel(issuerid, 4);
    gShotPoliceTick[issuerid] = gettime();
    return 1;
}
