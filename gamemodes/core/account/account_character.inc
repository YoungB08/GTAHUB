/*
    account_character.inc
    ----------------------
    Hệ thống Multi-Character (Đa Nhân Vật IC) cho Server RP.

    Flow đăng nhập sau khi tích hợp:
        1. Player kết nối   -> Kiểm tra tài khoản (đăng ký / đăng nhập bình thường)
        2. Đăng nhập OK     -> Hiện Character Selection Dialog
        3. Chọn nhân vật    -> Load stats của nhân vật đó vào slot playerid
        4. Chơi bình thường -> Mọi save/load đều dùng char_id, không dùng u_id trực tiếp
        5. Thoát game       -> Lưu vị trí + stats nhân vật về DB

    Giới hạn:
        MAX_CHARS_PER_ACCOUNT (2) – Một tài khoản tạo tối đa 2 nhân vật IC.

    API công khai:
        Char_GetID(playerid)              -> char_id đang chơi
        Char_GetFirstname(playerid)       -> Firstname IC
        Char_GetLastname(playerid)        -> Lastname IC
        Char_GetFullname(playerid, buf[]) -> "Firstname Lastname"
        Char_IsActive(playerid)           -> bool – đã chọn nhân vật chưa
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_CHARS_PER_ACCOUNT   (2)     // Tối đa 2 nhân vật IC / tài khoản
#define CHAR_FIRSTNAME_LEN      (32)
#define CHAR_LASTNAME_LEN       (32)

// Skin mặc định để chọn khi tạo nhân vật
static const DefaultMaleSkins[]   = {0, 1, 2, 7, 9, 26, 29, 34, 46, 58, 68, 72};
static const DefaultFemaleSkins[] = {9, 10, 13, 31, 38, 41, 53, 55, 63, 71, 73, 87};

// Spawn mặc định nếu không có vị trí lưu sẵn (Bệnh viện LS)
#define CHAR_DEFAULT_SPAWN_X    (1543.4)
#define CHAR_DEFAULT_SPAWN_Y    (-1675.6)
#define CHAR_DEFAULT_SPAWN_Z    (13.5)
#define CHAR_DEFAULT_SPAWN_A    (180.0)

// =============================================================================
// DATA
// =============================================================================

enum E_CHAR_DATA {
    E_CHAR_ID,                          // char_id trong DB
    E_CHAR_FIRSTNAME[CHAR_FIRSTNAME_LEN],
    E_CHAR_LASTNAME[CHAR_LASTNAME_LEN],
    E_CHAR_AGE,
    E_CHAR_GENDER,                      // 0=Nam, 1=Nữ
    E_CHAR_SKIN,
    E_CHAR_MONEY,
    E_CHAR_KILLS,
    E_CHAR_DEATHS,
    E_CHAR_JOB,
    E_CHAR_CLASS,
    E_CHAR_XP,
    Float:E_CHAR_X,
    Float:E_CHAR_Y,
    Float:E_CHAR_Z,
    Float:E_CHAR_A,
    bool:E_CHAR_ACTIVE              // Đã chọn và đang chơi với nhân vật này
}

static PlayerChar[MAX_PLAYERS][E_CHAR_DATA];

// Danh sách nhân vật của tài khoản hiện tại (tối đa MAX_CHARS_PER_ACCOUNT)
static CharList[MAX_PLAYERS][MAX_CHARS_PER_ACCOUNT][E_CHAR_DATA];
static CharListCount[MAX_PLAYERS];

// =============================================================================
// PREPARED STATEMENTS
// =============================================================================

static
    Statement:stmt_loadChars,
    Statement:stmt_insertChar,
    Statement:stmt_saveCharPos,
    Statement:stmt_saveCharStats,
    Statement:stmt_deleteChar,
    Statement:stmt_checkName;

// =============================================================================
// HOOKS – MYSQL SETUP
// =============================================================================

hook OnMySQLConnected()
{
    static const loadQ[] = "\
    SELECT \
        char_id, firstname, lastname, age, gender, skin_id, \
        money, kills, deaths, job_id, class_id, xp, \
        pos_x, pos_y, pos_z, pos_a \
    FROM \
        characters \
    WHERE \
        u_id = ? \
    ORDER BY \
        last_played DESC \
    LIMIT ?";
    stmt_loadChars = MySQL_PrepareStatement(MySQL_GetHandle(), loadQ);

    static const insertQ[] = "\
    INSERT INTO characters \
        (u_id, firstname, lastname, age, gender, skin_id) \
    VALUES \
        (?, ?, ?, ?, ?, ?)";
    stmt_insertChar = MySQL_PrepareStatement(MySQL_GetHandle(), insertQ);

    static const savePosQ[] = "\
    UPDATE characters SET \
        pos_x=?, pos_y=?, pos_z=?, pos_a=?, last_played=NOW() \
    WHERE char_id=?";
    stmt_saveCharPos = MySQL_PrepareStatement(MySQL_GetHandle(), savePosQ);

    static const saveStatsQ[] = "\
    UPDATE characters SET \
        money=?, kills=?, deaths=?, job_id=?, class_id=?, xp=?, \
        pos_x=?, pos_y=?, pos_z=?, pos_a=?, last_played=NOW() \
    WHERE char_id=?";
    stmt_saveCharStats = MySQL_PrepareStatement(MySQL_GetHandle(), saveStatsQ);

    static const deleteQ[] = "DELETE FROM characters WHERE char_id=? AND u_id=?";
    stmt_deleteChar = MySQL_PrepareStatement(MySQL_GetHandle(), deleteQ);

    static const checkNameQ[] = "SELECT char_id FROM characters WHERE firstname=? AND lastname=? LIMIT 1";
    stmt_checkName = MySQL_PrepareStatement(MySQL_GetHandle(), checkNameQ);

    return 1;
}

// =============================================================================
// HOOKS – PLAYER LIFECYCLE
// =============================================================================

hook OnPlayerConnect(playerid)
{
    Char_ResetData(playerid);
    return 1;
}

// Sau khi login thành công -> hiện character selection
hook OnPlayerLogin(playerid)
{
    // Thay vì spawn ngay, load danh sách nhân vật trước
    Char_LoadList(playerid);
    return 1;
}

// Lưu khi thoát
hook OnPlayerDisconnect(playerid, reason)
{
    if (PlayerChar[playerid][E_CHAR_ACTIVE])
        Char_SaveStats(playerid);
    return 1;
}

// AutoSave mỗi 3 phút
hook OnPlayerAutoSave(playerid)
{
    if (PlayerChar[playerid][E_CHAR_ACTIVE])
        Char_SavePosition(playerid);
    return 1;
}

// =============================================================================
// INTERNAL – RESET
// =============================================================================

static Char_ResetData(playerid)
{
    PlayerChar[playerid][E_CHAR_ID]       = 0;
    PlayerChar[playerid][E_CHAR_FIRSTNAME]= "";
    PlayerChar[playerid][E_CHAR_LASTNAME] = "";
    PlayerChar[playerid][E_CHAR_AGE]      = 20;
    PlayerChar[playerid][E_CHAR_GENDER]   = 0;
    PlayerChar[playerid][E_CHAR_SKIN]     = 0;
    PlayerChar[playerid][E_CHAR_MONEY]    = 0;
    PlayerChar[playerid][E_CHAR_KILLS]    = 0;
    PlayerChar[playerid][E_CHAR_DEATHS]   = 0;
    PlayerChar[playerid][E_CHAR_JOB]      = 0;
    PlayerChar[playerid][E_CHAR_CLASS]    = 0;
    PlayerChar[playerid][E_CHAR_XP]       = 0;
    PlayerChar[playerid][E_CHAR_X]        = CHAR_DEFAULT_SPAWN_X;
    PlayerChar[playerid][E_CHAR_Y]        = CHAR_DEFAULT_SPAWN_Y;
    PlayerChar[playerid][E_CHAR_Z]        = CHAR_DEFAULT_SPAWN_Z;
    PlayerChar[playerid][E_CHAR_A]        = CHAR_DEFAULT_SPAWN_A;
    PlayerChar[playerid][E_CHAR_ACTIVE]   = false;
    CharListCount[playerid] = 0;
}

// =============================================================================
// INTERNAL – LOAD CHAR LIST FROM DB (sau login)
// =============================================================================

static Char_LoadList(playerid)
{
    CharListCount[playerid] = 0;

    inline OnCharListLoad()
    {
        new charID, firstname[32], lastname[32], age, gender, skin;
        new money, kills, deaths, jobID, classID, xp;
        new Float:px, Float:py, Float:pz, Float:pa;

        MySQL_BindResultInt(stmt_loadChars,   0, charID);
        MySQL_BindResultString(stmt_loadChars, 1, firstname, 32);
        MySQL_BindResultString(stmt_loadChars, 2, lastname,  32);
        MySQL_BindResultInt(stmt_loadChars,   3, age);
        MySQL_BindResultInt(stmt_loadChars,   4, gender);
        MySQL_BindResultInt(stmt_loadChars,   5, skin);
        MySQL_BindResultInt(stmt_loadChars,   6, money);
        MySQL_BindResultInt(stmt_loadChars,   7, kills);
        MySQL_BindResultInt(stmt_loadChars,   8, deaths);
        MySQL_BindResultInt(stmt_loadChars,   9, jobID);
        MySQL_BindResultInt(stmt_loadChars,   10, classID);
        MySQL_BindResultInt(stmt_loadChars,   11, xp);
        MySQL_BindResultFloat(stmt_loadChars, 12, px);
        MySQL_BindResultFloat(stmt_loadChars, 13, py);
        MySQL_BindResultFloat(stmt_loadChars, 14, pz);
        MySQL_BindResultFloat(stmt_loadChars, 15, pa);

        new idx = 0;
        while (MySQL_Statement_FetchRow(stmt_loadChars) && idx < MAX_CHARS_PER_ACCOUNT)
        {
            CharList[playerid][idx][E_CHAR_ID]     = charID;
            strcopy(CharList[playerid][idx][E_CHAR_FIRSTNAME], firstname, 32);
            strcopy(CharList[playerid][idx][E_CHAR_LASTNAME],  lastname,  32);
            CharList[playerid][idx][E_CHAR_AGE]    = age;
            CharList[playerid][idx][E_CHAR_GENDER] = gender;
            CharList[playerid][idx][E_CHAR_SKIN]   = skin;
            CharList[playerid][idx][E_CHAR_MONEY]  = money;
            CharList[playerid][idx][E_CHAR_KILLS]  = kills;
            CharList[playerid][idx][E_CHAR_DEATHS] = deaths;
            CharList[playerid][idx][E_CHAR_JOB]    = jobID;
            CharList[playerid][idx][E_CHAR_CLASS]  = classID;
            CharList[playerid][idx][E_CHAR_XP]     = xp;
            CharList[playerid][idx][E_CHAR_X]      = px;
            CharList[playerid][idx][E_CHAR_Y]      = py;
            CharList[playerid][idx][E_CHAR_Z]      = pz;
            CharList[playerid][idx][E_CHAR_A]      = pa;
            idx++;

            // Refetch for next row
            MySQL_BindResultInt(stmt_loadChars,   0, charID);
            MySQL_BindResultString(stmt_loadChars, 1, firstname, 32);
            MySQL_BindResultString(stmt_loadChars, 2, lastname, 32);
            MySQL_BindResultInt(stmt_loadChars,   3, age);
            MySQL_BindResultInt(stmt_loadChars,   4, gender);
            MySQL_BindResultInt(stmt_loadChars,   5, skin);
            MySQL_BindResultInt(stmt_loadChars,   6, money);
            MySQL_BindResultInt(stmt_loadChars,   7, kills);
            MySQL_BindResultInt(stmt_loadChars,   8, deaths);
            MySQL_BindResultInt(stmt_loadChars,   9, jobID);
            MySQL_BindResultInt(stmt_loadChars,   10, classID);
            MySQL_BindResultInt(stmt_loadChars,   11, xp);
            MySQL_BindResultFloat(stmt_loadChars, 12, px);
            MySQL_BindResultFloat(stmt_loadChars, 13, py);
            MySQL_BindResultFloat(stmt_loadChars, 14, pz);
            MySQL_BindResultFloat(stmt_loadChars, 15, pa);
        }

        CharListCount[playerid] = idx;

        // Sau khi load xong -> hiện Selection Dialog
        Char_ShowSelectionDialog(playerid);
    }

    MySQL_BindInt(stmt_loadChars, 0, Player_GetAccountID(playerid));
    MySQL_BindInt(stmt_loadChars, 1, MAX_CHARS_PER_ACCOUNT);
    MySQL_ExecuteParallel_Inline(stmt_loadChars, using inline OnCharListLoad);
}

// =============================================================================
// UI – Character Selection Dialog
// =============================================================================

static Char_ShowSelectionDialog(playerid)
{
    new content[512], line[128];

    if (CharListCount[playerid] == 0)
    {
        content = "Ban chua co nhan vat nao!\nHay tao nhan vat moi de bat dau hanh trinh RP.";
    }
    else
    {
        for (new i = 0; i < CharListCount[playerid]; i++)
        {
            format(line, sizeof(line), "%s %s\t(Level %d | %s)\n",
                CharList[playerid][i][E_CHAR_FIRSTNAME],
                CharList[playerid][i][E_CHAR_LASTNAME],
                CharList[playerid][i][E_CHAR_XP] / 1000 + 1,   // Approx level from XP
                (CharList[playerid][i][E_CHAR_GENDER] == 0) ? "Nam" : "Nu"
            );
            strcat(content, line, sizeof(content));
        }
    }

    // Thêm dòng "Tạo nhân vật mới" nếu còn slot
    if (CharListCount[playerid] < MAX_CHARS_PER_ACCOUNT)
        strcat(content, "> Tao nhan vat moi\t(Slot trong)\n", sizeof(content));

    inline OnSelectChar(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response)
        {
            // ESC / Thoát
            Kick(playerid);
            return;
        }

        // Chọn nhân vật tồn tại
        if (listitem < CharListCount[playerid])
        {
            Char_Activate(playerid, listitem);
        }
        else
        {
            // Tạo nhân vật mới
            Char_ShowCreateDialog(playerid);
        }
    }

    Dialog_ShowCallback(
        playerid,
        using inline OnSelectChar,
        DIALOG_STYLE_LIST,
        "Chon Nhan Vat",
        content,
        "Chon",
        "Thoat"
    );
}

// =============================================================================
// UI – Character Creation Dialog
// =============================================================================

static Char_ShowCreateDialog(playerid)
{
    inline OnEnterName(response, listitem, string:inputtext[])
    {
        #pragma unused listitem
        if (!response)
        {
            Char_ShowSelectionDialog(playerid);
            return;
        }

        // Validate định dạng Firstname_Lastname
        new firstname[32], lastname[32];
        if (sscanf(inputtext, "s[32]s[32]", firstname, lastname))
        {
            SendErrorMsg(playerid, "Sai dinh dang! Phai la: Ten Ho (cach nhau bang dau cach).");
            Char_ShowCreateDialog(playerid);
            return;
        }

        // Kiểm tra độ dài
        if (strlen(firstname) < 2 || strlen(lastname) < 2)
        {
            SendErrorMsg(playerid, "Ten va ho phai co it nhat 2 ky tu!");
            Char_ShowCreateDialog(playerid);
            return;
        }

        // Kiểm tra tên đã dùng chưa
        Char_CheckNameAndCreate(playerid, firstname, lastname);
    }

    Dialog_ShowCallback(
        playerid,
        using inline OnEnterName,
        DIALOG_STYLE_INPUT,
        "Tao Nhan Vat Moi – Ten IC",
        "Nhap ten nhan vat theo dinh dang:\n"
        "{FFAA00}Firstname Lastname{FFFFFF}\n\n"
        "Vi du: {33AA33}Nguyen Van{FFFFFF} hoac {33AA33}John Smith\n\n"
        "{AFAFAF}Luu y: Ten nhan vat la duy nhat tren server.",
        "Tiep tuc",
        "Quay lai"
    );
}

static Char_CheckNameAndCreate(playerid, const firstname[], const lastname[])
{
    inline OnNameCheck()
    {
        new existID;
        MySQL_BindResultInt(stmt_checkName, 0, existID);

        if (MySQL_Statement_FetchRow(stmt_checkName) && existID > 0)
        {
            SendErrorMsg(playerid, "Ten nay da duoc su dung! Vui long chon ten khac.");
            Char_ShowCreateDialog(playerid);
            return;
        }

        // Tên hợp lệ -> hỏi tuổi & giới tính
        Char_ShowAgeGenderDialog(playerid, firstname, lastname);
    }

    MySQL_BindString(stmt_checkName, 0, firstname, 32);
    MySQL_BindString(stmt_checkName, 1, lastname,  32);
    MySQL_ExecuteParallel_Inline(stmt_checkName, using inline OnNameCheck);
}

static Char_ShowAgeGenderDialog(playerid, const firstname[], const lastname[])
{
    new content[256];
    format(content, sizeof(content),
        "Nhan vat: {33AA33}%s %s{FFFFFF}\n\n"
        "Chon gioi tinh:\n"
        "Nam\t(Co the chon skin Nam)\n"
        "Nu\t(Co the chon skin Nu)\n",
        firstname, lastname
    );

    inline OnGenderSelect(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response)
        {
            Char_ShowCreateDialog(playerid);
            return;
        }

        new gender = listitem; // 0=Nam, 1=Nu

        // Hỏi tuổi
        inline OnAgeInput(response2, listitem2, string:ageText[])
        {
            #pragma unused listitem2
            if (!response2)
            {
                Char_ShowAgeGenderDialog(playerid, firstname, lastname);
                return;
            }

            new age;
            if (sscanf(ageText, "i", age) || age < 18 || age > 70)
            {
                SendErrorMsg(playerid, "Tuoi khong hop le! Phai tu 18 den 70.");
                Char_ShowAgeGenderDialog(playerid, firstname, lastname);
                return;
            }

            Char_ShowSkinDialog(playerid, firstname, lastname, age, gender);
        }

        Dialog_ShowCallback(playerid, using inline OnAgeInput,
            DIALOG_STYLE_INPUT,
            "Tao Nhan Vat – Tuoi",
            "Nhap tuoi nhan vat (18-70):",
            "Tiep tuc", "Quay lai");
    }

    Dialog_ShowCallback(playerid, using inline OnGenderSelect,
        DIALOG_STYLE_LIST,
        "Tao Nhan Vat – Gioi Tinh",
        content,
        "Chon", "Quay lai");
}

static Char_ShowSkinDialog(playerid, const firstname[], const lastname[], age, gender)
{
    new content[256], line[24];

    if (gender == 0)
    {
        for (new i = 0; i < sizeof(DefaultMaleSkins); i++)
        {
            format(line, sizeof(line), "Skin #%d\n", DefaultMaleSkins[i]);
            strcat(content, line, sizeof(content));
        }
    }
    else
    {
        for (new i = 0; i < sizeof(DefaultFemaleSkins); i++)
        {
            format(line, sizeof(line), "Skin #%d\n", DefaultFemaleSkins[i]);
            strcat(content, line, sizeof(content));
        }
    }

    inline OnSkinSelect(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response)
        {
            Char_ShowAgeGenderDialog(playerid, firstname, lastname);
            return;
        }

        new skinID;
        if (gender == 0) skinID = DefaultMaleSkins[listitem];
        else             skinID = DefaultFemaleSkins[listitem];

        // Tạo nhân vật vào DB
        Char_InsertToDB(playerid, firstname, lastname, age, gender, skinID);
    }

    Dialog_ShowCallback(playerid, using inline OnSkinSelect,
        DIALOG_STYLE_LIST,
        "Tao Nhan Vat – Skin",
        content,
        "Chon skin", "Quay lai");
}

// =============================================================================
// INTERNAL – Insert Character to DB
// =============================================================================

static Char_InsertToDB(playerid, const firstname[], const lastname[], age, gender, skinID)
{
    inline OnCharInserted()
    {
        new charID = cache_insert_id();

        // Thêm vào CharList runtime
        new idx = CharListCount[playerid];
        CharList[playerid][idx][E_CHAR_ID]     = charID;
        strcopy(CharList[playerid][idx][E_CHAR_FIRSTNAME], firstname, 32);
        strcopy(CharList[playerid][idx][E_CHAR_LASTNAME],  lastname,  32);
        CharList[playerid][idx][E_CHAR_AGE]    = age;
        CharList[playerid][idx][E_CHAR_GENDER] = gender;
        CharList[playerid][idx][E_CHAR_SKIN]   = skinID;
        CharList[playerid][idx][E_CHAR_MONEY]  = 5000;
        CharList[playerid][idx][E_CHAR_X]      = CHAR_DEFAULT_SPAWN_X;
        CharList[playerid][idx][E_CHAR_Y]      = CHAR_DEFAULT_SPAWN_Y;
        CharList[playerid][idx][E_CHAR_Z]      = CHAR_DEFAULT_SPAWN_Z;
        CharList[playerid][idx][E_CHAR_A]      = CHAR_DEFAULT_SPAWN_A;
        CharListCount[playerid]++;

        SendMsgF(playerid, COLOR_GREEN, "[NHAN VAT] Da tao nhan vat %s %s thanh cong!", firstname, lastname);
        Char_Activate(playerid, idx);
    }

    MySQL_BindInt(stmt_insertChar, 0, Player_GetAccountID(playerid));
    MySQL_BindString(stmt_insertChar, 1, firstname, 32);
    MySQL_BindString(stmt_insertChar, 2, lastname,  32);
    MySQL_BindInt(stmt_insertChar, 3, age);
    MySQL_BindInt(stmt_insertChar, 4, gender);
    MySQL_BindInt(stmt_insertChar, 5, skinID);
    MySQL_ExecuteThreaded_Inline(stmt_insertChar, using inline OnCharInserted);
}

// =============================================================================
// INTERNAL – Activate Character (áp dụng stats vào player)
// =============================================================================

static Char_Activate(playerid, listIdx)
{
    // Copy từ CharList -> PlayerChar
    PlayerChar[playerid][E_CHAR_ID]        = CharList[playerid][listIdx][E_CHAR_ID];
    strcopy(PlayerChar[playerid][E_CHAR_FIRSTNAME], CharList[playerid][listIdx][E_CHAR_FIRSTNAME], 32);
    strcopy(PlayerChar[playerid][E_CHAR_LASTNAME],  CharList[playerid][listIdx][E_CHAR_LASTNAME],  32);
    PlayerChar[playerid][E_CHAR_AGE]       = CharList[playerid][listIdx][E_CHAR_AGE];
    PlayerChar[playerid][E_CHAR_GENDER]    = CharList[playerid][listIdx][E_CHAR_GENDER];
    PlayerChar[playerid][E_CHAR_SKIN]      = CharList[playerid][listIdx][E_CHAR_SKIN];
    PlayerChar[playerid][E_CHAR_MONEY]     = CharList[playerid][listIdx][E_CHAR_MONEY];
    PlayerChar[playerid][E_CHAR_KILLS]     = CharList[playerid][listIdx][E_CHAR_KILLS];
    PlayerChar[playerid][E_CHAR_DEATHS]    = CharList[playerid][listIdx][E_CHAR_DEATHS];
    PlayerChar[playerid][E_CHAR_JOB]       = CharList[playerid][listIdx][E_CHAR_JOB];
    PlayerChar[playerid][E_CHAR_CLASS]     = CharList[playerid][listIdx][E_CHAR_CLASS];
    PlayerChar[playerid][E_CHAR_XP]        = CharList[playerid][listIdx][E_CHAR_XP];
    PlayerChar[playerid][E_CHAR_X]         = CharList[playerid][listIdx][E_CHAR_X];
    PlayerChar[playerid][E_CHAR_Y]         = CharList[playerid][listIdx][E_CHAR_Y];
    PlayerChar[playerid][E_CHAR_Z]         = CharList[playerid][listIdx][E_CHAR_Z];
    PlayerChar[playerid][E_CHAR_A]         = CharList[playerid][listIdx][E_CHAR_A];
    PlayerChar[playerid][E_CHAR_ACTIVE]    = true;

    // Áp dụng cho player
    SetPlayerSkin(playerid, PlayerChar[playerid][E_CHAR_SKIN]);
    GivePlayerMoney(playerid, PlayerChar[playerid][E_CHAR_MONEY]);
    Player_SetJob(playerid, PlayerChar[playerid][E_CHAR_JOB]);
    Player_SetClass(playerid, PlayerChar[playerid][E_CHAR_CLASS]);
    Player_GiveKill(playerid, PlayerChar[playerid][E_CHAR_KILLS]);
    Player_SetDeaths(playerid, PlayerChar[playerid][E_CHAR_DEATHS]);

    // Spawn tại vị trí nhân vật
    SetSpawnInfo(playerid, NO_TEAM, PlayerChar[playerid][E_CHAR_SKIN],
        PlayerChar[playerid][E_CHAR_X], PlayerChar[playerid][E_CHAR_Y],
        PlayerChar[playerid][E_CHAR_Z], PlayerChar[playerid][E_CHAR_A],
        0, 0, 0, 0, 0, 0);
    SpawnPlayer(playerid);

    SendMsgF(playerid, COLOR_GREEN,
        "[NHAN VAT] Chao mung %s %s! Chuc ban choi vui!",
        PlayerChar[playerid][E_CHAR_FIRSTNAME],
        PlayerChar[playerid][E_CHAR_LASTNAME]);

    CallLocalFunction("OnCharacterActivated", "i", playerid);
}

// =============================================================================
// SAVE – Position only
// =============================================================================

static Char_SavePosition(playerid)
{
    new Float:px, Float:py, Float:pz, Float:pa;
    GetPlayerPos(playerid, px, py, pz);
    GetPlayerFacingAngle(playerid, pa);

    MySQL_BindFloat(stmt_saveCharPos, 0, px);
    MySQL_BindFloat(stmt_saveCharPos, 1, py);
    MySQL_BindFloat(stmt_saveCharPos, 2, pz);
    MySQL_BindFloat(stmt_saveCharPos, 3, pa);
    MySQL_BindInt(stmt_saveCharPos, 4, PlayerChar[playerid][E_CHAR_ID]);
    MySQL_ExecuteThreaded(stmt_saveCharPos);
}

// =============================================================================
// SAVE – Full stats
// =============================================================================

static Char_SaveStats(playerid)
{
    new Float:px, Float:py, Float:pz, Float:pa;
    GetPlayerPos(playerid, px, py, pz);
    GetPlayerFacingAngle(playerid, pa);

    MySQL_BindInt(stmt_saveCharStats,   0, GetPlayerMoney(playerid));
    MySQL_BindInt(stmt_saveCharStats,   1, Player_GetKills(playerid));
    MySQL_BindInt(stmt_saveCharStats,   2, Player_GetDeaths(playerid));
    MySQL_BindInt(stmt_saveCharStats,   3, Player_GetJob(playerid));
    MySQL_BindInt(stmt_saveCharStats,   4, Player_GetClass(playerid));
    MySQL_BindInt(stmt_saveCharStats,   5, Player_GetXP(playerid));
    MySQL_BindFloat(stmt_saveCharStats, 6, px);
    MySQL_BindFloat(stmt_saveCharStats, 7, py);
    MySQL_BindFloat(stmt_saveCharStats, 8, pz);
    MySQL_BindFloat(stmt_saveCharStats, 9, pa);
    MySQL_BindInt(stmt_saveCharStats,   10, PlayerChar[playerid][E_CHAR_ID]);
    MySQL_ExecuteThreaded(stmt_saveCharStats);
}

// =============================================================================
// PUBLIC API
// =============================================================================

stock Char_GetID(playerid)
    return PlayerChar[playerid][E_CHAR_ID];

stock bool:Char_IsActive(playerid)
    return PlayerChar[playerid][E_CHAR_ACTIVE];

stock Char_GetFirstname(playerid, buf[], len = sizeof(buf))
    strcopy(buf, PlayerChar[playerid][E_CHAR_FIRSTNAME], len);

stock Char_GetLastname(playerid, buf[], len = sizeof(buf))
    strcopy(buf, PlayerChar[playerid][E_CHAR_LASTNAME], len);

stock Char_GetFullname(playerid, buf[], len = sizeof(buf))
{
    format(buf, len, "%s %s",
        PlayerChar[playerid][E_CHAR_FIRSTNAME],
        PlayerChar[playerid][E_CHAR_LASTNAME]);
}

stock Char_GetAge(playerid)
    return PlayerChar[playerid][E_CHAR_AGE];

stock Char_GetGender(playerid)
    return PlayerChar[playerid][E_CHAR_GENDER];

// =============================================================================
// COMMAND – /charinfo – Xem thông tin nhân vật hiện tại
// =============================================================================

CMD:charinfo(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid) || !Char_IsActive(playerid))
        return SendErrorMsg(playerid, "Ban chua chon nhan vat."), 1;

    new content[512];
    format(content, sizeof(content),
        "=== HO SO NHAN VAT IC ===\n\n"
        "Ten IC:     %s %s\n"
        "Tuoi:       %d\n"
        "Gioi tinh:  %s\n"
        "Job:        %d\n"
        "Class:      %d\n"
        "XP:         %d\n\n"
        "Tien mat:   $%d",
        PlayerChar[playerid][E_CHAR_FIRSTNAME],
        PlayerChar[playerid][E_CHAR_LASTNAME],
        PlayerChar[playerid][E_CHAR_AGE],
        (PlayerChar[playerid][E_CHAR_GENDER] == 0) ? "Nam" : "Nu",
        Player_GetJob(playerid),
        Player_GetClass(playerid),
        Player_GetXP(playerid),
        GetPlayerMoney(playerid)
    );

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX,
        "Thong Tin Nhan Vat IC", content, "Dong", "");
    return 1;
}

// Alias /id
ALTCMD:mychar(playerid, params);

// =============================================================================
// COMMAND – /switchchar – Đổi nhân vật (từ selection dialog)
// =============================================================================

CMD:switchchar(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    // Lưu nhân vật hiện tại trước
    if (Char_IsActive(playerid))
        Char_SaveStats(playerid);

    // Reset và load lại list
    Char_ResetData(playerid);
    Char_LoadList(playerid);
    return 1;
}

// =============================================================================
// COMMAND – /deletechar – Xóa nhân vật
// =============================================================================

CMD:deletechar(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (!Char_IsActive(playerid))
        return SendErrorMsg(playerid, "Ban chua co nhan vat nao dang hoat dong."), 1;

    inline OnConfirmDelete(response, listitem, string:inputtext[])
    {
        #pragma unused listitem, inputtext
        if (!response) return;

        new charID = PlayerChar[playerid][E_CHAR_ID];

        MySQL_BindInt(stmt_deleteChar, 0, charID);
        MySQL_BindInt(stmt_deleteChar, 1, Player_GetAccountID(playerid));
        MySQL_ExecuteThreaded(stmt_deleteChar);

        SendMsgF(playerid, COLOR_RED, "[NHAN VAT] Da xoa nhan vat %s %s.",
            PlayerChar[playerid][E_CHAR_FIRSTNAME],
            PlayerChar[playerid][E_CHAR_LASTNAME]);

        // Quay về chọn nhân vật
        Char_ResetData(playerid);
        Char_LoadList(playerid);
    }

    Dialog_ShowCallback(playerid, using inline OnConfirmDelete,
        DIALOG_STYLE_MSGBOX,
        "Xac Nhan Xoa Nhan Vat",
        "Ban co chac muon XOA VINH VIEN nhan vat nay?\n"
        "{E85454}Hanh dong nay KHONG THE hoan tac!",
        "Xac nhan xoa", "Huy");
    return 1;
}
