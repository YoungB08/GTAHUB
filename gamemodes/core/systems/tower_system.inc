#include <YSI_Coding\y_hooks>
#include <YSI_Data\y_iterate>

static
    TowerCP,
    TowerRobbedRecently,
    BriefCase
;
new
    Timer:FindingTimer[MAX_PLAYERS]
;

enum E_BRIEFCASE_DATA {
    Float:E_BRIEFCASE_X,
    Float:E_BRIEFCASE_Y,
    Float:E_BRIEFCASE_Z
}

static const RandomBriefCase[][E_BRIEFCASE_DATA] =
{
    {1831.9935, -1274.5226, 125.7266},
    {1788.1812, -1309.2687, 125.7266},
    {1816.2598, -1299.8323, 125.7266},
    {1774.2363,-1301.4355,125.7209}
};

Briefcase_SetRandomLoc(playerid) {
    new rand = random(sizeof(RandomBriefCase));
    robbingdone[playerid] = 1;
    DestroyPickup(BriefCase);
    BriefCase = CreatePickup(1210, 1, RandomBriefCase[rand][E_BRIEFCASE_X], RandomBriefCase[rand][E_BRIEFCASE_Y], RandomBriefCase[rand][E_BRIEFCASE_Z], 0);
}

hook OnGameModeInit()
{
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    CreateObject(1652, 1788.966796, -1297.323242, 12.930883, 0, 0, 177.300064, 0.0); //fence
    CreateObject(2332, 1817.233276, -1306.228027, 97.952163, 0, 0, 0, 0.0); //safe
    TowerCP = CreateDynamicCP(1817.1315, -1307.1591, 98.5000, 1.0, 0, 0, .streamdistance = CHECKPOINT_STREAMDISTANCE);
    return 1;
}

hook OnPlayerEnterDynamicCP(playerid, checkpointid)
{
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    if(checkpointid == TowerCP)
    {
        if(robbing[playerid] == 1) return 0;
        SendServerMsg(playerid, "Dung /towerrobbery de cuop toa thap!");
    }
    return 1;
}

hook OnPlayerLeaveDynamicCP(playerid, checkpointid)
{
    if(FreeRoleplay_IsEnabled()) {
        return 1;
    }

    if(checkpointid == TowerCP) {
        if(robbing[playerid] == 1 && BurglaryInfo[playerid] == 4) {
            if(robbingdone[playerid] == 1) return 0;
            SendServerMsg(playerid, "Ban da roi khoi checkpoint, vu cuop bi huy!");
            Player_StopRobbery(playerid);
            UI_HideTimeLeft(playerid);
            defer TowerCooldown();
        }
    }
    return 1;
}

hook OnPlayerPickUpPickup(playerid, pickupid) {
    if(pickupid == BriefCase && BurglaryInfo[playerid] == 4) {
        SetHideoutTimer(playerid);

        DestroyPickup(BriefCase);

        stop FindingTimer[playerid];
    }
    return 1;
}

CMD:towerrobbery(playerid) {
    if(Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendServerMsg(playerid, "Chi dan thuong moi co the cuop toa thap!");
    }
    if(!IsPlayerInRangeOfPoint(playerid, 7.0, 1817.1315, -1307.1591, 98.5000)) {
        return SendServerMsg(playerid, "Ban khong o gan ket sat!");
    }
    if(TowerRobbedRecently == 1) {
        return SendServerMsg(playerid, "Toa thap vua bi cuop gan day!");
    }
    if(BurglaryInfo[playerid] != 0) {
        return SendServerMsg(playerid, "Ban dang trong mot vu cuop, hay den noi an nap!");
    }
    robbing[playerid] = 1;
    BurglaryInfo[playerid] = 4;
    TowerRobbedRecently = 1;
    SendServerMsg(playerid, "Dang cuop van phong, hay dung trong diem danh dau mau do!");
    ApplyAnimation(playerid, "SHOP", "ROB_Shifty", 4.0, false, false, false, false, 5000, SYNC_NONE);
    burglarytimer[playerid] = defer SetBriefCaseLoc(playerid);
    UI_TimeLeft(playerid, 20);
    return 1;
}

timer SetBriefCaseLoc[20000](playerid)
{
    Player_GiveWantedLevel(playerid, 6);
    ClearAnimations(playerid, SYNC_ALL);
    robbingdone[playerid] = 1;
    Briefcase_SetRandomLoc(playerid);
    FindingTimer[playerid] = defer TowerFailedToGetBC(playerid);
    UI_TimeLeft(playerid, 60);
    SendServerMsg(playerid, "Ban co 60 giay de lay vali tren tang thuong.");
    return 1;
}

timer TowerFailedToGetBC[60000](playerid)
{
    Player_GiveWantedLevel(playerid, 6);
    DestroyPickup(BriefCase);
    robbing[playerid] = 0;
    BurglaryInfo[playerid] = 0;
    robbingdone[playerid] = 0;
    UI_HideTimeLeft(playerid);
    defer TowerCooldown();
    return 1;
}

timer TowerCooldown[RobberyCD]()
{
    TowerRobbedRecently = 0;
}
