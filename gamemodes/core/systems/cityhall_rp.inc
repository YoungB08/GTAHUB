/*
    cityhall_rp.inc
    ----------------
    Nâng cấp City Hall RP: Biển số xe IC & Giấy phép kinh doanh.

    Tính năng:
    - Đăng ký biển số xe IC tại City Hall NPC (/platereg)
    - Đổi biển số xe (/changeplate)
    - Đăng ký giấy phép kinh doanh (/bizlicense) – bước đầu để mở shop
    - Xem danh sách giấy phép kinh doanh của mình (/mybiz)
    - NPC City Hall tại vị trí gốc (VW 26, pos gốc của cityhall_system.inc)
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define PLATE_REG_COST          (5000)      // Phí đăng ký biển số xe
#define PLATE_CHANGE_COST       (2000)      // Phí đổi biển số
#define PLATE_MAX_LENGTH        (8)         // Tối đa 8 ký tự

#define BIZ_LICENSE_COST        (25000)     // Phí đăng ký GPDKD

// City Hall NPC location
#define CITYHALL_NPC_X          (359.7139)
#define CITYHALL_NPC_Y          (175.5916)  // Sát cạnh NPC đổi nghề đã có
#define CITYHALL_NPC_Z          (1008.3893)
#define CITYHALL_NPC_VW         (26)
#define CITYHALL_NPC_RADIUS     (5.0)

// =============================================================================
// DATA
// =============================================================================

static
    Statement:stmt_savePlate,
    Statement:stmt_loadPlate,
    Statement:stmt_insertBizLicense,
    Statement:stmt_checkBizLicense;

// Biển số xe runtime (indexed by SAMP vehicleid)
static VehiclePlate[MAX_VEHICLES][PLATE_MAX_LENGTH + 1];

// Đang có GPDKD?
static bool:PlayerHasBizLicense[MAX_PLAYERS];

// =============================================================================
// HOOKS – MYSQL SETUP
// =============================================================================

hook OnMySQLConnected()
{
    // Thêm cột plate vào player_vehicles nếu chưa có
    mysql_query_file(MySQL_GetHandle(), "player_vehicles_plate.sql", false);

    // Thêm bảng biz_licenses nếu chưa có
    mysql_query(MySQL_GetHandle(),
        "CREATE TABLE IF NOT EXISTS biz_licenses (\
            id int(11) NOT NULL AUTO_INCREMENT,\
            u_id int(11) NOT NULL,\
            biz_name varchar(64) NOT NULL,\
            registered_at datetime DEFAULT CURRENT_TIMESTAMP,\
            PRIMARY KEY (id),\
            FOREIGN KEY (u_id) REFERENCES players(u_id) ON DELETE CASCADE\
        )");

    static const savePlateQ[] = "UPDATE player_vehicles SET plate=? WHERE vehicle_id=?";
    stmt_savePlate = MySQL_PrepareStatement(MySQL_GetHandle(), savePlateQ);

    static const loadPlateQ[] = "SELECT vehicle_id, plate FROM player_vehicles WHERE u_id=?";
    stmt_loadPlate = MySQL_PrepareStatement(MySQL_GetHandle(), loadPlateQ);

    static const insertBizQ[] = "INSERT INTO biz_licenses (u_id, biz_name) VALUES (?, ?)";
    stmt_insertBizLicense = MySQL_PrepareStatement(MySQL_GetHandle(), insertBizQ);

    static const checkBizQ[] = "SELECT id FROM biz_licenses WHERE u_id=? LIMIT 1";
    stmt_checkBizLicense = MySQL_PrepareStatement(MySQL_GetHandle(), checkBizQ);

    return 1;
}

// =============================================================================
// HOOKS – PLAYER LIFECYCLE
// =============================================================================

hook OnPlayerLogin(playerid)
{
    CityHall_LoadPlates(playerid);
    CityHall_LoadBizLicense(playerid);
    return 1;
}

hook OnPlayerConnect(playerid)
{
    PlayerHasBizLicense[playerid] = false;
    return 1;
}

// =============================================================================
// HOOK – CITYHALL NPC INTERACTION
// =============================================================================

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if (PRESSED(KEY_YES))
    {
        if (IsPlayerInRangeOfPoint(playerid, CITYHALL_NPC_RADIUS,
            CITYHALL_NPC_X, CITYHALL_NPC_Y, CITYHALL_NPC_Z)
            && GetPlayerVirtualWorld(playerid) == CITYHALL_NPC_VW)
        {
            CityHall_ShowMainMenu(playerid);
        }
    }
    return 1;
}

// =============================================================================
// INIT – NPC + Label
// =============================================================================

hook OnGameModeInit()
{
    Create3DTextLabel(
        "Nhan "C_YELLOW"~k~~CONVERSATION_YES~"C_WHITE" de vao "C_SAMPBLUE"Toa Thi Chinh",
        COLOR_WHITE, CITYHALL_NPC_X, CITYHALL_NPC_Y, CITYHALL_NPC_Z, 40.0, CITYHALL_NPC_VW, false);

    new npcActor = CreateActor(143, CITYHALL_NPC_X, CITYHALL_NPC_Y, CITYHALL_NPC_Z - 1.0, 90.0);
    SetActorVirtualWorld(npcActor, CITYHALL_NPC_VW);
    return 1;
}

// =============================================================================
// MENU CHÍNH CITY HALL
// =============================================================================

static CityHall_ShowMainMenu(playerid)
{
    static const content[] =
        "Dang ky bien so xe IC\t$5,000\n\
Doi bien so xe\t$2,000\n\
Cap Giay Phep Kinh Doanh (GPDKD)\t$25,000\n\
Xem giay phep cua toi\tMien phi";

    inline OnMenuSelect(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response) return 1;

        switch (listitem)
        {
            case 0: CityHall_PlateReg(playerid);
            case 1: CityHall_ChangePlate(playerid);
            case 2: CityHall_BizLicense(playerid);
            case 3: CityHall_ViewLicenses(playerid);
        }
        return 1;
    }

    new menuContent[256];
    format(menuContent, sizeof(menuContent), "Dich Vu\tPhi\n%s", content);

    Dialog_ShowCallback(playerid, using inline OnMenuSelect,
        DIALOG_STYLE_TABLIST_HEADERS,
        "Toa Thi Chinh – Dich Vu",
        menuContent,
        "Chon", "Dong");
    return 1;
}

// =============================================================================
// LOAD PLATES
// =============================================================================

static CityHall_LoadPlates(playerid)
{
    inline OnPlatesLoaded()
    {
        new vehicleID, plate[PLATE_MAX_LENGTH + 1];
        MySQL_BindResultInt(stmt_loadPlate, 0, vehicleID);
        MySQL_BindResultString(stmt_loadPlate, 1, plate, PLATE_MAX_LENGTH + 1);

        while (MySQL_Statement_FetchRow(stmt_loadPlate))
        {
            // Apply plate to SAMP vehicle
            foreach (new i : PersonalVehicle)
            {
                if (Vehicle_GetUID(i) == vehicleID)
                {
                    new handler = Vehicle_GetHandler(i);
                    if (handler && !isnull(plate))
                    {
                        strcopy(VehiclePlate[handler], plate, PLATE_MAX_LENGTH + 1);
                        SetVehicleNumberPlate(handler, plate);
                    }
                    break;
                }
            }

            MySQL_BindResultInt(stmt_loadPlate, 0, vehicleID);
            MySQL_BindResultString(stmt_loadPlate, 1, plate, PLATE_MAX_LENGTH + 1);
        }
    }

    MySQL_BindInt(stmt_loadPlate, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadPlate, using inline OnPlatesLoaded);
}

// =============================================================================
// LOAD BIZ LICENSE
// =============================================================================

static CityHall_LoadBizLicense(playerid)
{
    inline OnBizLicenseLoad()
    {
        new id;
        MySQL_BindResultInt(stmt_checkBizLicense, 0, id);
        PlayerHasBizLicense[playerid] = (MySQL_Statement_FetchRow(stmt_checkBizLicense) && id > 0);
    }

    MySQL_BindInt(stmt_checkBizLicense, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_checkBizLicense, using inline OnBizLicenseLoad);
}

// =============================================================================
// ĐĂNG KÝ BIỂN SỐ
// =============================================================================

static CityHall_PlateReg(playerid)
{
    if (GetPlayerMoney(playerid) < PLATE_REG_COST)
        return SendErrorMsgF(playerid, "Can $%d de dang ky bien so.", PLATE_REG_COST), void:0;

    // Tìm xe cá nhân gần nhất của người chơi
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new nearVehUID = -1;
    new nearHandler = -1;
    new Float:minDist = 20.0;

    foreach (new i : PersonalVehicle)
    {
        if (!Vehicle_IsOwner(playerid, Vehicle_GetHandler(i))) continue;
        new Float:vx, Float:vy, Float:vz;
        GetVehiclePos(Vehicle_GetHandler(i), vx, vy, vz);
        new Float:d = Distance(px, py, pz, vx, vy, vz);
        if (d < minDist)
        {
            minDist = d;
            nearVehUID = Vehicle_GetUID(i);
            nearHandler = Vehicle_GetHandler(i);
        }
    }

    if (nearVehUID == -1)
        return SendErrorMsg(playerid, "Khong tim thay xe ca nhan nao trong pham vi 20m."), void:0;

    inline OnPlateInput(response, listitem, string:inputtext[])
    {
        #pragma unused listitem
        if (!response) return 1;

        if (strlen(inputtext) < 2 || strlen(inputtext) > PLATE_MAX_LENGTH)
        {
            SendErrorMsgF(playerid, "Bien so phai tu 2-8 ky tu!");
            return 1;
        }

        // Chuyển thành chữ hoa
        new plateTxt[PLATE_MAX_LENGTH + 1];
        strcopy(plateTxt, inputtext, PLATE_MAX_LENGTH + 1);
        for (new c = 0; c < strlen(plateTxt); c++)
            if ('a' <= plateTxt[c] <= 'z') plateTxt[c] -= 32;

        // Apply
        SetVehicleNumberPlate(nearHandler, plateTxt);
        strcopy(VehiclePlate[nearHandler], plateTxt, PLATE_MAX_LENGTH + 1);

        // Save
        MySQL_BindString(stmt_savePlate, 0, plateTxt, PLATE_MAX_LENGTH + 1);
        MySQL_BindInt(stmt_savePlate, 1, nearVehUID);
        MySQL_ExecuteThreaded(stmt_savePlate);

        GivePlayerMoney(playerid, -PLATE_REG_COST);
        SendMsgF(playerid, COLOR_GREEN, "[BIEN SO] Da dang ky bien so '%s' cho xe cua ban! (-$%d)", plateTxt, PLATE_REG_COST);
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline OnPlateInput,
        DIALOG_STYLE_INPUT,
        "Dang Ky Bien So Xe IC",
        "Nhap bien so xe mong muon (2-8 ky tu):\n\
{AFAFAF}Vi du: 51B-12345 hoac ABC-999\n\n\
{FFAA00}Phi: $5,000",
        "Xac nhan", "Huy");
    return 1;
}

// =============================================================================
// ĐỔI BIỂN SỐ
// =============================================================================

static CityHall_ChangePlate(playerid)
{
    if (GetPlayerMoney(playerid) < PLATE_CHANGE_COST)
        return SendErrorMsgF(playerid, "Can $%d de doi bien so.", PLATE_CHANGE_COST), void:0;

    CityHall_PlateReg(playerid); // Dùng lại flow đăng ký, chỉ khác phí
    return 1;
}

// =============================================================================
// GIẤY PHÉP KINH DOANH
// =============================================================================

static CityHall_BizLicense(playerid)
{
    if (PlayerHasBizLicense[playerid])
        return SendErrorMsg(playerid, "Ban da co Giay Phep Kinh Doanh roi!"), void:0;

    if (GetPlayerMoney(playerid) < BIZ_LICENSE_COST)
        return SendErrorMsgF(playerid, "Can $%d de dang ky GPDKD.", BIZ_LICENSE_COST), void:0;

    inline OnBizNameInput(response, listitem, string:inputtext[])
    {
        #pragma unused listitem
        if (!response) return 1;

        if (strlen(inputtext) < 3 || strlen(inputtext) > 48)
        {
            SendErrorMsg(playerid, "Ten thuong ngan phai tu 3-48 ky tu!");
            return 1;
        }

        GivePlayerMoney(playerid, -BIZ_LICENSE_COST);
        PlayerHasBizLicense[playerid] = true;

        MySQL_BindInt(stmt_insertBizLicense, 0, Player_GetAccountID(playerid));
        MySQL_BindString(stmt_insertBizLicense, 1, inputtext, 48);
        MySQL_ExecuteThreaded(stmt_insertBizLicense);

        SendMsgF(playerid, COLOR_GREEN,
            "[GPDKD] Da cap Giay Phep Kinh Doanh cho '%s'. Chuc kinh doanh phat dat! (-$%d)",
            inputtext, BIZ_LICENSE_COST);
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline OnBizNameInput,
        DIALOG_STYLE_INPUT,
        "Dang Ky Giay Phep Kinh Doanh",
        "Nhap ten thuong hieu / cua hang (3-48 ky tu):\n\n\
{FFAA00}Phi dang ky: $25,000",
        "Dang ky", "Huy");
    return 1;
}

// =============================================================================
// XEM GIẤY PHÉP
// =============================================================================

static CityHall_ViewLicenses(playerid)
{
    new content[512];

    // Bằng lái xe
    format(content, sizeof(content),
        "Bang lai xe (Car):    %s\n\
Bang lai xe tai:      %s\n\
Bang lai may bay:     %s\n\
Giay phep sung:       %s\n\
Giay phep KD (GPDKD): %s",
        Player_HasLicense(playerid, LICENSE_CAR)   ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}",
        Player_HasLicense(playerid, LICENSE_TRUCK) ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}",
        Player_HasLicense(playerid, LICENSE_PLANE) ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}",
        Player_HasLicense(playerid, LICENSE_GUN)   ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}",
        PlayerHasBizLicense[playerid] ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}"
    );

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Giay To – Ho So Ca Nhan", content, "Dong", "");
    return 1;
}

// =============================================================================
// COMMAND – /platereg – Đăng ký biển số xe (shortcut, không cần vào City Hall)
// =============================================================================

CMD:platereg(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    // Phải gần City Hall
    if (!IsPlayerInRangeOfPoint(playerid, 50.0, CITYHALL_NPC_X, CITYHALL_NPC_Y, CITYHALL_NPC_Z)
        || GetPlayerVirtualWorld(playerid) != CITYHALL_NPC_VW)
        return SendErrorMsg(playerid, "Ban phai o trong Toa Thi Chinh de dang ky bien so xe."), 1;

    CityHall_PlateReg(playerid);
    return 1;
}

// =============================================================================
// COMMAND – /bizlicense – Đăng ký GPDKD
// =============================================================================

CMD:bizlicense(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    CityHall_BizLicense(playerid);
    return 1;
}

// =============================================================================
// COMMAND – /mybiz – Xem giấy phép kinh doanh
// =============================================================================

CMD:mybiz(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (!PlayerHasBizLicense[playerid])
        return SendErrorMsg(playerid, "Ban chua co Giay Phep Kinh Doanh. Hay den Toa Thi Chinh."), 1;

    SendMsgF(playerid, COLOR_GREEN, "[GPDKD] Ban dang so huu Giay Phep Kinh Doanh hop le.");
    return 1;
}

// =============================================================================
// PUBLIC API
// =============================================================================

stock Player_HasBizLicense(playerid)
{
    return PlayerHasBizLicense[playerid];
}
