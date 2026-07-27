#include <YSI_Coding\y_hooks>

#define LICENSE_NONE            (-1)

enum {
    LICENSE_DRIVING = 0,
    LICENSE_TRUCK,
    LICENSE_PILOT,
    LICENSE_WEAPON,
    LICENSE_JOB,
    LICENSE_TOTAL
}

static
    bool:PlayerLicense[MAX_PLAYERS][LICENSE_TOTAL],
    Statement:stmt_loadLicenses,
    Statement:stmt_insertLicense,
    Statement:stmt_deleteLicense;

static bool:License_IsValid(licenseid)
{
    return (0 <= licenseid < LICENSE_TOTAL);
}

static bool:License_IsAircraftModel(modelid)
{
    switch(modelid) {
        case 417, 425, 447, 460, 469, 476, 487, 488, 497, 511, 512, 513, 519, 520, 548, 553, 563, 577, 592, 593:
            return true;
    }
    return false;
}

static bool:License_IsHeavyVehicleModel(modelid)
{
    switch(modelid) {
        case 403, 406, 407, 408, 414, 431, 433, 437, 443, 455, 456, 486, 498, 499, 514, 515, 524, 525, 552, 578, 609:
            return true;
    }
    return false;
}

hook OnMySQLConnected()
{
    stmt_loadLicenses = MySQL_PrepareStatement(
        MySQL_GetHandle(),
        "SELECT license_id FROM player_licenses WHERE u_id = ?"
    );

    stmt_insertLicense = MySQL_PrepareStatement(
        MySQL_GetHandle(),
        "INSERT IGNORE INTO player_licenses (u_id, license_id) VALUES (?, ?)"
    );

    stmt_deleteLicense = MySQL_PrepareStatement(
        MySQL_GetHandle(),
        "DELETE FROM player_licenses WHERE u_id = ? AND license_id = ?"
    );
    return 1;
}

hook OnPlayerConnect(playerid)
{
    for(new i; i < LICENSE_TOTAL; i++) {
        PlayerLicense[playerid][i] = false;
    }
    return 1;
}

hook OnPlayerLogin(playerid)
{
    License_Load(playerid);
    return 1;
}

static License_Load(playerid)
{
    inline OnLicenseLoad()
    {
        new licenseid;

        MySQL_BindResultInt(stmt_loadLicenses, 0, licenseid);

        while(MySQL_Statement_FetchRow(stmt_loadLicenses)) {
            if(License_IsValid(licenseid)) {
                PlayerLicense[playerid][licenseid] = true;
            }
        }
    }

    MySQL_BindInt(stmt_loadLicenses, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadLicenses, using inline OnLicenseLoad);
    return 1;
}

stock License_GetName(licenseid, output[], len)
{
    switch(licenseid) {
        case LICENSE_DRIVING: format(output, len, "Bang lai xe");
        case LICENSE_TRUCK: format(output, len, "Bang xe tai/bus");
        case LICENSE_PILOT: format(output, len, "Bang may bay");
        case LICENSE_WEAPON: format(output, len, "Giay phep sung");
        case LICENSE_JOB: format(output, len, "Giay phep nghe dac biet");
        default: format(output, len, "Khong xac dinh");
    }
    return 1;
}

stock bool:License_PlayerHas(playerid, licenseid)
{
    if(!IsPlayerConnected(playerid) || !License_IsValid(licenseid)) {
        return false;
    }
    return PlayerLicense[playerid][licenseid];
}

stock License_Give(playerid, licenseid, bool:save = true)
{
    if(!IsPlayerConnected(playerid) || !License_IsValid(licenseid)) {
        return 0;
    }

    PlayerLicense[playerid][licenseid] = true;

    if(save && Player_GetAccountID(playerid) > 0) {
        MySQL_BindInt(stmt_insertLicense, 0, Player_GetAccountID(playerid));
        MySQL_BindInt(stmt_insertLicense, 1, licenseid);
        MySQL_ExecuteThreaded(stmt_insertLicense);
    }
    return 1;
}

stock License_Remove(playerid, licenseid)
{
    if(!IsPlayerConnected(playerid) || !License_IsValid(licenseid)) {
        return 0;
    }

    PlayerLicense[playerid][licenseid] = false;

    if(Player_GetAccountID(playerid) > 0) {
        MySQL_BindInt(stmt_deleteLicense, 0, Player_GetAccountID(playerid));
        MySQL_BindInt(stmt_deleteLicense, 1, licenseid);
        MySQL_ExecuteThreaded(stmt_deleteLicense);
    }
    return 1;
}

stock License_GetRequiredForVehicleModel(modelid)
{
    if(License_IsAircraftModel(modelid)) {
        return LICENSE_PILOT;
    }

    if(License_IsHeavyVehicleModel(modelid)) {
        return LICENSE_TRUCK;
    }

    return LICENSE_DRIVING;
}

stock bool:License_CheckVehicleAccess(playerid, vehicleid, bool:notify = true)
{
    if(!IsPlayerConnected(playerid) || vehicleid == INVALID_VEHICLE_ID) {
        return false;
    }

    if(GetPVarInt(playerid, "LicenseTestActive")) {
        return true;
    }

    new
        modelid = GetVehicleModel(vehicleid),
        licenseid = License_GetRequiredForVehicleModel(modelid);

    if(License_PlayerHas(playerid, licenseid)) {
        return true;
    }

    if(notify) {
        new licenseName[32];
        License_GetName(licenseid, licenseName, sizeof(licenseName));
        SendErrorMsgF(playerid, "Ban can %s de lai phuong tien nay.", licenseName);
    }
    return false;
}

stock License_BuildList(playerid, output[], len)
{
    output[0] = '\0';

    new
        licenseName[32],
        line[96];

    strcat(output, "Giay phep\tTrang thai\n", len);

    for(new i; i < LICENSE_TOTAL; i++) {
        License_GetName(i, licenseName, sizeof(licenseName));
        format(
            line,
            sizeof(line),
            "%s\t%s\n",
            licenseName,
            License_PlayerHas(playerid, i) ? (C_GREEN"Da co") : (C_RED"Chua co")
        );
        strcat(output, line, len);
    }
    return 1;
}

stock License_ShowList(playerid, targetid = INVALID_PLAYER_ID)
{
    if(targetid == INVALID_PLAYER_ID) {
        targetid = playerid;
    }

    new
        title[64],
        list[512],
        playerName[MAX_PLAYER_NAME];

    GetPlayerName(targetid, playerName, sizeof(playerName));
    format(title, sizeof(title), "Giay phep - %s", playerName);
    License_BuildList(targetid, list, sizeof(list));
    Dialog_Show(playerid, DIALOG_STYLE_TABLIST_HEADERS, title, list, "Dong", "");
    return 1;
}

CMD:licenses(playerid, params[])
{
    new targetid;

    if(sscanf(params, "u", targetid)) {
        targetid = playerid;
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong hop le.");
    }

    License_ShowList(playerid, targetid);
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    #pragma unused oldstate

    if(newstate == PLAYER_STATE_DRIVER) {
        new vehicleid = GetPlayerVehicleID(playerid);

        if(!License_CheckVehicleAccess(playerid, vehicleid)) {
            RemovePlayerFromVehicle(playerid);
        }
    }
    return 1;
}
