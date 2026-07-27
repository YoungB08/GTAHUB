#include <YSI_Coding\y_hooks>

#define DRIVING_TEST_AREA_RADIUS        (3.0)
#define DRIVING_TEST_SCORE_START        (100)
#define DRIVING_TEST_SCORE_MIN          (70)
#define DRIVING_TEST_TIMEOUT            (240)

enum E_DRIVING_TEST_DATA {
    E_TEST_LICENSE,
    E_TEST_NAME[32],
    E_TEST_FEE,
    E_TEST_MODEL,
    Float:E_TEST_VEH_X,
    Float:E_TEST_VEH_Y,
    Float:E_TEST_VEH_Z,
    Float:E_TEST_VEH_A
}

static const DrivingTestData[][E_DRIVING_TEST_DATA] = {
    {LICENSE_DRIVING, "Bang lai xe", 1500, 410, 1486.7281, -1737.4581, 13.2268, 89.0},
    {LICENSE_TRUCK, "Bang xe tai/bus", 2500, 414, 1486.7281, -1737.4581, 13.2268, 89.0},
    {LICENSE_PILOT, "Bang may bay", 3500, 593, 1964.6954, -2493.8677, 13.5469, 90.0},
    {LICENSE_WEAPON, "Giay phep sung", 5000, 0, 0.0, 0.0, 0.0, 0.0},
    {LICENSE_JOB, "Giay phep nghe dac biet", 3000, 0, 0.0, 0.0, 0.0, 0.0}
};

static const Float:DrivingTestRoute[][3] = {
    {1515.3986, -1737.8516, 13.3828},
    {1547.1158, -1694.4685, 13.3828},
    {1472.6682, -1593.1599, 13.3828},
    {1395.6273, -1626.2689, 13.3828},
    {1481.4891, -1738.7125, 13.3828}
};

static const Float:PilotTestRoute[][3] = {
    {1810.6481, -2493.4907, 13.5469},
    {1665.7125, -2492.2354, 13.5469},
    {1545.4658, -2492.2234, 13.5469},
    {1964.6954, -2493.8677, 13.5469}
};

static
    DrivingTestArea,
    bool:DrivingTestActive[MAX_PLAYERS],
    DrivingTestID[MAX_PLAYERS],
    DrivingTestVehicle[MAX_PLAYERS],
    DrivingTestCheckpoint[MAX_PLAYERS],
    DrivingTestScore[MAX_PLAYERS],
    DrivingTestDeadline[MAX_PLAYERS],
    Float:DrivingTestVehicleHealth[MAX_PLAYERS];

static bool:DrivingTest_IsActive(playerid)
{
    return DrivingTestActive[playerid];
}

static DrivingTest_GetRoutePoint(testid, step, &Float:x, &Float:y, &Float:z)
{
    if(DrivingTestData[testid][E_TEST_LICENSE] == LICENSE_PILOT) {
        if(step >= sizeof(PilotTestRoute)) {
            return 0;
        }

        x = PilotTestRoute[step][0];
        y = PilotTestRoute[step][1];
        z = PilotTestRoute[step][2];
        return 1;
    }

    if(step >= sizeof(DrivingTestRoute)) {
        return 0;
    }

    x = DrivingTestRoute[step][0];
    y = DrivingTestRoute[step][1];
    z = DrivingTestRoute[step][2];
    return 1;
}

static DrivingTest_GetRouteSize(testid)
{
    if(DrivingTestData[testid][E_TEST_LICENSE] == LICENSE_PILOT) {
        return sizeof(PilotTestRoute);
    }
    return sizeof(DrivingTestRoute);
}

static DrivingTest_SetNextCheckpoint(playerid)
{
    new
        testid = DrivingTestID[playerid],
        step = DrivingTestCheckpoint[playerid],
        Float:x,
        Float:y,
        Float:z;

    if(!DrivingTest_GetRoutePoint(testid, step, x, y, z)) {
        return 0;
    }

    SetPlayerRaceCheckpoint(playerid, CP_TYPE_GROUND_FINISH, x, y, z, x, y, z, 4.0);
    SendServerMsgF(playerid, "Bai thi: diem %d/%d. Di cham va giu xe an toan.", step + 1, DrivingTest_GetRouteSize(testid));
    return 1;
}

static DrivingTest_End(playerid, bool:passed, const reason[] = "")
{
    if(!DrivingTest_IsActive(playerid)) {
        return 0;
    }

    new
        testid = DrivingTestID[playerid],
        vehicleid = DrivingTestVehicle[playerid];

    DisablePlayerRaceCheckpoint(playerid);
    DeletePVar(playerid, "LicenseTestActive");

    if(IsValidVehicle(vehicleid)) {
        DestroyVehicle(vehicleid);
    }

    DrivingTestActive[playerid] = false;
    DrivingTestID[playerid] = -1;
    DrivingTestVehicle[playerid] = INVALID_VEHICLE_ID;
    DrivingTestCheckpoint[playerid] = 0;
    DrivingTestDeadline[playerid] = 0;

    TogglePlayerControllable(playerid, true);

    if(passed) {
        new licenseid = DrivingTestData[testid][E_TEST_LICENSE];
        License_Give(playerid, licenseid, true);
        SendServerMsgF(playerid, "Ban da dat bai thi va nhan %s.", DrivingTestData[testid][E_TEST_NAME]);
        PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
    }
    else if(!isnull(reason)) {
        SendErrorMsgF(playerid, "Bai thi that bai: %s", reason);
    }
    return 1;
}

static DrivingTest_StartPractical(playerid, testid)
{
    if(DrivingTest_IsActive(playerid)) {
        return SendErrorMsg(playerid, "Ban dang lam bai thi khac.");
    }

    if(GetPlayerMoney(playerid) < DrivingTestData[testid][E_TEST_FEE]) {
        return SendErrorMsg(playerid, "Ban khong du tien dong le phi thi.");
    }

    GivePlayerMoney(playerid, -DrivingTestData[testid][E_TEST_FEE]);

    if(DrivingTestData[testid][E_TEST_MODEL] == 0) {
        License_Give(playerid, DrivingTestData[testid][E_TEST_LICENSE], true);
        SendServerMsgF(playerid, "Ban da nop le phi va nhan %s.", DrivingTestData[testid][E_TEST_NAME]);
        return 1;
    }

    new vehicleid = CreateVehicle(
        DrivingTestData[testid][E_TEST_MODEL],
        DrivingTestData[testid][E_TEST_VEH_X],
        DrivingTestData[testid][E_TEST_VEH_Y],
        DrivingTestData[testid][E_TEST_VEH_Z],
        DrivingTestData[testid][E_TEST_VEH_A],
        1,
        1,
        -1,
        false
    );

    DrivingTestActive[playerid] = true;
    DrivingTestID[playerid] = testid;
    DrivingTestVehicle[playerid] = vehicleid;
    DrivingTestCheckpoint[playerid] = 0;
    DrivingTestScore[playerid] = DRIVING_TEST_SCORE_START;
    DrivingTestDeadline[playerid] = gettime() + DRIVING_TEST_TIMEOUT;
    DrivingTestVehicleHealth[playerid] = 1000.0;

    SetPVarInt(playerid, "LicenseTestActive", 1);
    SetPlayerPos(playerid, DrivingTestData[testid][E_TEST_VEH_X], DrivingTestData[testid][E_TEST_VEH_Y], DrivingTestData[testid][E_TEST_VEH_Z] + 1.0);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerInterior(playerid, 0);
    PutPlayerInVehicle(playerid, vehicleid, 0);
    GetVehicleHealth(vehicleid, DrivingTestVehicleHealth[playerid]);
    DrivingTest_SetNextCheckpoint(playerid);
    return 1;
}

static DrivingTest_ShowTheory(playerid, testid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        if(listitem != 0) {
            SendErrorMsg(playerid, "Ban tra loi sai phan ly thuyet.");
            return 1;
        }

        DrivingTest_StartPractical(playerid, testid);
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        DrivingTestData[testid][E_TEST_NAME],
        "Giam toc, quan sat va di dung lan duong.\nTang toc qua nga tu neu duong vang.\nRoi xe giua bai thi de tiet kiem thoi gian.",
        "Chon",
        "Huy"
    );
    return 1;
}

static DrivingTest_ShowMenu(playerid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        if(listitem < 0 || listitem >= sizeof(DrivingTestData)) {
            return 1;
        }

        if(License_PlayerHas(playerid, DrivingTestData[listitem][E_TEST_LICENSE])) {
            SendErrorMsg(playerid, "Ban da co giay phep nay.");
            return 1;
        }

        DrivingTest_ShowTheory(playerid, listitem);
        return 1;
    }

    new list[384], line[80];
    list[0] = '\0';

    for(new i; i < sizeof(DrivingTestData); i++) {
        format(line, sizeof(line), "%s\t"C_GREEN"%m\n", DrivingTestData[i][E_TEST_NAME], DrivingTestData[i][E_TEST_FEE]);
        strcat(list, line, sizeof(list));
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_TABLIST,
        "Dang ky thi giay phep",
        list,
        "Thi",
        "Dong"
    );
    return 1;
}

hook OnGameModeInit()
{
    new label[160];
    Interaction_BuildPointLabel(label, sizeof(label), "Cong viec", "Trung tam sat hach", "Yeu cau le phi va bai thi", "dang ky thi");

    DrivingTestArea = CreateDynamicCircle(1476.8500, -1738.9000, DRIVING_TEST_AREA_RADIUS, 0, 0);
    CreateDynamic3DTextLabel(
        label,
        COLOR_ORANGE,
        1476.8500,
        -1738.9000,
        14.5469,
        INTERACTION_LABEL_DISTANCE,
        .streamdistance = INTERACTION_LABEL_DISTANCE
    );
    return 1;
}

hook OnPlayerConnect(playerid)
{
    DrivingTestActive[playerid] = false;
    DrivingTestID[playerid] = -1;
    DrivingTestVehicle[playerid] = INVALID_VEHICLE_ID;
    DrivingTestCheckpoint[playerid] = 0;
    DrivingTestScore[playerid] = 0;
    DrivingTestDeadline[playerid] = 0;
    DeletePVar(playerid, "LicenseTestActive");
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    #pragma unused reason

    if(DrivingTest_IsActive(playerid)) {
        DrivingTest_End(playerid, false);
    }
    return 1;
}

hook OnPlayerEnterDynArea(playerid, areaid)
{
    if(areaid == DrivingTestArea) {
        Interaction_ShowPrompt(playerid, INTERACTION_TYPE_JOB, 9000, "~y~~h~Trung tam sat hach", "dang ky thi");
    }
    return 1;
}

hook OnPlayerLeaveDynArea(playerid, areaid)
{
    if(areaid == DrivingTestArea) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_JOB, 9000);
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(PRESSED(KEY_NO) && Interaction_IsActive(playerid, INTERACTION_TYPE_JOB, 9000)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_JOB, 9000);
        return 1;
    }

    if(PRESSED(KEY_YES) && Interaction_IsActive(playerid, INTERACTION_TYPE_JOB, 9000)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_JOB, 9000);
        DrivingTest_ShowMenu(playerid);
    }
    return 1;
}

hook OnPlayerEnterRaceCP(playerid)
{
    if(!DrivingTest_IsActive(playerid)) {
        return 1;
    }

    DrivingTestCheckpoint[playerid]++;

    if(DrivingTestCheckpoint[playerid] >= DrivingTest_GetRouteSize(DrivingTestID[playerid])) {
        return DrivingTest_End(playerid, DrivingTestScore[playerid] >= DRIVING_TEST_SCORE_MIN, "Diem so khong dat yeu cau.");
    }

    DrivingTest_SetNextCheckpoint(playerid);
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    #pragma unused oldstate

    if(DrivingTest_IsActive(playerid) && newstate == PLAYER_STATE_ONFOOT) {
        DrivingTest_End(playerid, false, "Ban da roi khoi xe.");
    }
    return 1;
}

static ptask DrivingTest_Watcher[1000](playerid)
{
    if(!DrivingTest_IsActive(playerid)) {
        return;
    }

    if(gettime() > DrivingTestDeadline[playerid]) {
        DrivingTest_End(playerid, false, "Qua thoi gian.");
        return;
    }

    new vehicleid = DrivingTestVehicle[playerid];

    if(!IsValidVehicle(vehicleid) || GetPlayerVehicleID(playerid) != vehicleid) {
        DrivingTest_End(playerid, false, "Ban khong con trong xe thi.");
        return;
    }

    new Float:health;
    GetVehicleHealth(vehicleid, health);

    if(health + 25.0 < DrivingTestVehicleHealth[playerid]) {
        DrivingTestScore[playerid] -= 10;
        DrivingTestVehicleHealth[playerid] = health;
        SendErrorMsgF(playerid, "Ban bi tru diem va cham. Diem hien tai: %d.", DrivingTestScore[playerid]);

        if(DrivingTestScore[playerid] < DRIVING_TEST_SCORE_MIN) {
            DrivingTest_End(playerid, false, "Va cham qua nhieu.");
        }
    }
}

CMD:drivingtest(playerid, params[])
{
    #pragma unused params
    DrivingTest_ShowMenu(playerid);
    return 1;
}
