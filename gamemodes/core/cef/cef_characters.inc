// =============================================================================
// CEF Characters - GTAHUB
// Xử lý chọn nhân vật & tạo nhân vật qua CEF browser
//
// Character Selection Events:
//   JS -> Pawn: GTAHUB:SelectCharacter(charName)
//   JS -> Pawn: GTAHUB:DeleteCharacter(charName)
//   JS -> Pawn: GTAHUB:OpenCreateCharacter
//   Pawn -> JS: GTAHUB:CharacterList (JSON array)
//
// Character Creation Events:
//   JS -> Pawn: GTAHUB:CreateCharacter(name, dob, gender, skinId, desc)
//   Pawn -> JS: GTAHUB:CreateCharacterResponse(success, message)
// =============================================================================

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_SelectCharacter(playerid, browserid, const charName[]);
forward OnCEF_DeleteCharacter(playerid, browserid, const charName[]);
forward OnCEF_OpenCreateCharacter(playerid, browserid);
forward OnCEF_CreateCharacter(playerid, browserid, const name[], const dob[], const gender[], skinId, const desc[]);
forward OnCEF_ToCharacterSelectionPage(playerid, browserid);
forward CEF_SendCharacterList(playerid);
forward CEF_CharCreate_Done(playerid);

// --- Đăng ký events ---
hook OnGameModeInit()
{
    // Character Selection events
    CEF_RegisterEvent("GTAHUB:SelectCharacter", "OnCEF_SelectCharacter",
        Argument_String     // charName
    );

    CEF_RegisterEvent("GTAHUB:DeleteCharacter", "OnCEF_DeleteCharacter",
        Argument_String     // charName
    );

    CEF_RegisterEvent("GTAHUB:OpenCreateCharacter", "OnCEF_OpenCreateCharacter");

    // Character Creation events
    CEF_RegisterEvent("GTAHUB:CreateCharacter", "OnCEF_CreateCharacter",
        Argument_String,    // name (Firstname_Lastname hoặc Firstname Lastname)
        Argument_String,    // dob (YYYY-MM-DD)
        Argument_String,    // gender ("male" / "female")
        Argument_Integer,   // skinId
        Argument_String     // desc (background)
    );

    CEF_RegisterEvent("GTAHUB:ToCharacterSelectionPage", "OnCEF_ToCharacterSelectionPage");

    print("[CEF] Character events registered.");
    return 1;
}

// =============================================================================
// CHARACTER SELECTION
// =============================================================================

// --- Mở Character Selection browser ---
stock CEF_ShowCharacterSelection(playerid)
{
    CEF_CloseAllBrowsers(playerid);
    CEF_DisableGameUI(playerid);
    CEF_CreateBrowser(playerid, CEF_BROWSER_CHAR_SELECT, CEF_URL_CHAR_SELECT, true, false);
    TogglePlayerSpectating(playerid, true);

    // Load và gửi data nhân vật thực tế cho JS
    SetTimerEx("CEF_SendCharacterList", 300, false, "i", playerid);
    return 1;
}

// --- Gửi danh sách nhân vật lên JS ---
public CEF_SendCharacterList(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    CEF_LoadAndEmitCharacters(playerid);
    return 1;
}

// --- Load nhân vật từ bảng characters và emit cho JS ---
static CEF_LoadAndEmitCharacters(playerid)
{
    new accountid = Player_GetAccountID(playerid);
    if (accountid <= 0)
    {
        accountid = Player_GetMasterAccountID(playerid);
    }
    if (accountid <= 0)
    {
        Kick(playerid);
        return 0;
    }

    inline const OnCharLoad()
    {
        new
            rows = cache_num_rows(),
            json[1536],
            entry[384],
            charId,
            firstName[32],
            lastName[32],
            age,
            gender,
            skinId,
            money,
            xp;

        strcat(json, "[", sizeof(json));

        for (new i = 0; i < rows; i++)
        {
            cache_get_value_name_int(i, "char_id", charId);
            cache_get_value_name(i, "firstname", firstName, sizeof(firstName));
            cache_get_value_name(i, "lastname", lastName, sizeof(lastName));
            cache_get_value_name_int(i, "age", age);
            cache_get_value_name_int(i, "gender", gender);
            cache_get_value_name_int(i, "skin_id", skinId);
            cache_get_value_name_int(i, "money", money);
            cache_get_value_name_int(i, "xp", xp);

            new level = xp / 1000 + 1;
            new fullName[64];
            format(fullName, sizeof(fullName), "%s_%s", firstName, lastName);

            format(entry, sizeof(entry),
                "{\"id\":%d,\"name\":\"%s\",\"level\":%d,\"cash\":%d,\"bank\":0,\"phone\":\"N/A\",\"skin\":%d,\"faction\":\"Dan Thuong\",\"vehicles\":[]}",
                charId,
                fullName,
                level,
                money,
                skinId
            );

            strcat(json, entry, sizeof(json));

            if (i < rows - 1)
                strcat(json, ",", sizeof(json));
        }

        strcat(json, "]", sizeof(json));

        printf("[CEF] CEF_LoadAndEmitCharacters: playerid=%d, rows=%d, json='%s'", playerid, rows, json);
        CEF_EmitEvent(playerid, CEF_BROWSER_CHAR_SELECT, "GTAHUB:CharacterList", CEF_STR(json));
    }

    MySQL_TQueryInline(
        MySQL_GetHandle(),
        using inline OnCharLoad,
        "SELECT char_id, firstname, lastname, age, gender, skin_id, money, xp FROM characters WHERE u_id = %i ORDER BY last_played DESC LIMIT 4",
        accountid
    );
    return 1;
}

// --- CEF Event: Chọn nhân vật để vào game ---
public OnCEF_SelectCharacter(playerid, browserid, const charName[])
{
    #pragma unused browserid

    if (!IsPlayerConnected(playerid))
        return 0;

    if (isnull(charName))
        return 1;

    // Phân tích firstname & lastname từ charName (hỗ trợ cả _ và space)
    new firstname[32], lastname[32];
    new sepPos = strfind(charName, "_");
    if (sepPos == -1)
        sepPos = strfind(charName, " ");

    if (sepPos != -1)
    {
        strmid(firstname, charName, 0, sepPos, sizeof(firstname));
        strmid(lastname, charName, sepPos + 1, strlen(charName), sizeof(lastname));
    }
    else
    {
        strcpy(firstname, charName, sizeof(firstname));
        lastname[0] = '\0';
    }

    inline const OnCharSelectFind()
    {
        if (!cache_num_rows())
        {
            SendErrorMsg(playerid, "Khong tim thay nhan vat nay!");
            return 1;
        }

        new charId, skinId, money, kills, deaths, jobId, classId, xp;
        new Float:px, Float:py, Float:pz, Float:pa;

        cache_get_value_name_int(0, "char_id", charId);
        cache_get_value_name_int(0, "skin_id", skinId);
        cache_get_value_name_int(0, "money", money);
        cache_get_value_name_int(0, "kills", kills);
        cache_get_value_name_int(0, "deaths", deaths);
        cache_get_value_name_int(0, "job_id", jobId);
        cache_get_value_name_int(0, "class_id", classId);
        cache_get_value_name_int(0, "xp", xp);
        cache_get_value_name_float(0, "pos_x", px);
        cache_get_value_name_float(0, "pos_y", py);
        cache_get_value_name_float(0, "pos_z", pz);
        cache_get_value_name_float(0, "pos_a", pa);

        // Đóng CEF browser & mở lại Game UI (Chat & HUD)
        CEF_CloseAllBrowsers(playerid);
        TogglePlayerSpectating(playerid, false);
        CEF_EnableGameUI(playerid);

        // Đổi tên SA-MP thành Firstname_Lastname
        new sampName[MAX_PLAYER_NAME];
        if (!isnull(lastname))
            format(sampName, sizeof(sampName), "%s_%s", firstname, lastname);
        else
            strcpy(sampName, firstname, sizeof(sampName));

        SetPlayerName(playerid, sampName);

        // Apply stats & spawn
        SetPlayerSkin(playerid, skinId);
        GivePlayerMoney(playerid, money);
        Player_SetJob(playerid, jobId);
        Player_SetClass(playerid, classId);

        if (px == 0.0 && py == 0.0)
        {
            px = 1543.4; py = -1675.6; pz = 13.5; pa = 180.0;
        }

        SetSpawnInfo(playerid, NO_TEAM, skinId, px, py, pz, pa, 0, 0, 0, 0, 0, 0);
        SpawnPlayer(playerid);

        SendServerMsgF(playerid, "Da vao nhan vat %s!", sampName);
        CallLocalFunction("OnCharacterActivated", "i", playerid);
        return 1;
    }

    new accountid = Player_GetAccountID(playerid);
    if (accountid <= 0) accountid = Player_GetMasterAccountID(playerid);

    if (!isnull(lastname))
    {
        MySQL_TQueryInline(
            MySQL_GetHandle(),
            using inline OnCharSelectFind,
            "SELECT char_id, skin_id, money, kills, deaths, job_id, class_id, xp, pos_x, pos_y, pos_z, pos_a \
             FROM characters WHERE u_id = %i AND firstname = '%e' AND lastname = '%e' LIMIT 1",
            accountid, firstname, lastname
        );
    }
    else
    {
        MySQL_TQueryInline(
            MySQL_GetHandle(),
            using inline OnCharSelectFind,
            "SELECT char_id, skin_id, money, kills, deaths, job_id, class_id, xp, pos_x, pos_y, pos_z, pos_a \
             FROM characters WHERE u_id = %i AND firstname = '%e' LIMIT 1",
            accountid, firstname
        );
    }
    return 1;
}

// --- CEF Event: Xóa nhân vật ---
public OnCEF_DeleteCharacter(playerid, browserid, const charName[])
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid))
        return 0;

    SendClientMessage(playerid, 0xFFAA00FF, "[CEF] Tinh nang xoa nhan vat chua duoc kich hoat.");
    return 1;
}

// --- CEF Event: Mở trang tạo nhân vật ---
public OnCEF_OpenCreateCharacter(playerid, browserid)
{
    #pragma unused browserid
    CEF_ShowCharacterCreation(playerid);
    return 1;
}

// --- CEF Event: Quay về character selection ---
public OnCEF_ToCharacterSelectionPage(playerid, browserid)
{
    #pragma unused browserid
    CEF_ShowCharacterSelection(playerid);
    return 1;
}

// =============================================================================
// CHARACTER CREATION
// =============================================================================

// --- Mở Character Creation browser ---
stock CEF_ShowCharacterCreation(playerid)
{
    CEF_CloseAllBrowsers(playerid);
    CEF_DisableGameUI(playerid);
    CEF_CreateBrowser(playerid, CEF_BROWSER_CHAR_CREATE, CEF_URL_CHAR_CREATE, true, false);
    TogglePlayerSpectating(playerid, true);
    return 1;
}

// --- Helper: Tính tuổi từ date string (YYYY-MM-DD) ---
static CEF_CalcAge(const dob[])
{
    new year = 0, month = 0, day = 0;
    sscanf(dob, "p<->iii", year, month, day);

    new currentYear = 2025;
    new age = currentYear - year;
    if (age < 18) age = 18;
    if (age > 80) age = 80;
    return age;
}

// --- CEF Event: Tạo nhân vật mới ---
public OnCEF_CreateCharacter(playerid, browserid, const name[], const dob[], const gender[], skinId, const desc[])
{
    #pragma unused browserid, desc

    if (!IsPlayerConnected(playerid))
        return 0;

    new accountid = Player_GetAccountID(playerid);
    if (accountid <= 0) accountid = Player_GetMasterAccountID(playerid);

    if (accountid <= 0)
    {
        Kick(playerid);
        return 0;
    }

    if (isnull(name))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_CHAR_CREATE, "GTAHUB:CreateCharacterResponse",
            CEF_INT(0), CEF_STR("Ten nhan vat khong duoc de trong!")
        );
        return 1;
    }

    // Phân tách firstname / lastname (hỗ trợ cả _ lẫn khoảng trắng)
    new firstname[32], lastname[32];
    new sepPos = strfind(name, "_");
    if (sepPos == -1) sepPos = strfind(name, " ");

    if (sepPos == -1)
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_CHAR_CREATE, "GTAHUB:CreateCharacterResponse",
            CEF_INT(0), CEF_STR("Sai dinh dang! Dinh dang dung: Firstname_Lastname")
        );
        return 1;
    }

    strmid(firstname, name, 0, sepPos, sizeof(firstname));
    strmid(lastname, name, sepPos + 1, strlen(name), sizeof(lastname));

    if (strlen(firstname) < 2 || strlen(lastname) < 2)
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_CHAR_CREATE, "GTAHUB:CreateCharacterResponse",
            CEF_INT(0), CEF_STR("Ten va Ho phai tu 2 ky tu tro len!")
        );
        return 1;
    }

    new age = CEF_CalcAge(dob);
    new genderInt = (strequal(gender, "female", true)) ? 1 : 0;

    // Kiểm tra tên đã tồn tại trong DB chưa
    inline const OnCheckNameExist()
    {
        if (cache_num_rows() > 0)
        {
            CEF_EmitEvent(playerid, CEF_BROWSER_CHAR_CREATE, "GTAHUB:CreateCharacterResponse",
                CEF_INT(0), CEF_STR("Ten nhan vat nay da duoc su dung!")
            );
            return 1;
        }

        // Gọi hàm tạo nhân vật trong account_character.inc
        Char_InsertToDB(playerid, firstname, lastname, age, genderInt, skinId);

        CEF_EmitEvent(playerid, CEF_BROWSER_CHAR_CREATE, "GTAHUB:CreateCharacterResponse",
            CEF_INT(1), CEF_STR("Tao nhan vat thanh cong! Dang chuyen...")
        );

        SetTimerEx("CEF_CharCreate_Done", 1500, false, "i", playerid);
        return 1;
    }

    MySQL_TQueryInline(
        MySQL_GetHandle(),
        using inline OnCheckNameExist,
        "SELECT char_id FROM characters WHERE firstname = '%e' AND lastname = '%e' LIMIT 1",
        firstname, lastname
    );
    return 1;
}

// --- Timer chuyển về character selection sau khi tạo xong ---
public CEF_CharCreate_Done(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_CREATE);
    CEF_ShowCharacterSelection(playerid);
    return 1;
}

// =============================================================================
// PUBLIC ALIAS FOR OTHER MODULES
// =============================================================================

stock Account_ShowCharacterMenu_CEF(playerid)
{
    CEF_ShowCharacterSelection(playerid);
    return 1;
}
