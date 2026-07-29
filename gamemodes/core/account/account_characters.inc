#include <YSI_Coding\y_hooks>

#define MAX_ACCOUNT_CHARACTERS     (4)
#define CHARACTER_PASSWORD_MARKER  "CHARACTER_SLOT"

enum {
    CHARACTER_GENDER_MALE = 0,
    CHARACTER_GENDER_FEMALE
}

static
    CharacterMenuCount[MAX_PLAYERS],
    CharacterMenuID[MAX_PLAYERS][MAX_ACCOUNT_CHARACTERS],
    CharacterID[MAX_PLAYERS],
    CharacterAge[MAX_PLAYERS],
    CharacterGender[MAX_PLAYERS],
    CharacterUsername[MAX_PLAYERS][MAX_PLAYER_NAME],
    CharacterFirstName[MAX_PLAYERS][24],
    CharacterLastName[MAX_PLAYERS][24],
    CharacterOrigin[MAX_PLAYERS][32],
    CharacterBackground[MAX_PLAYERS][96],
    CharacterCreateFirst[MAX_PLAYERS][24],
    CharacterCreateLast[MAX_PLAYERS][24],
    CharacterCreateAge[MAX_PLAYERS],
    CharacterCreateGender[MAX_PLAYERS],
    CharacterCreateOrigin[MAX_PLAYERS][32],
    CharacterCreateBackground[MAX_PLAYERS][96];

hook OnMySQLConnected()
{
    return 1;
}

static Character_ResetCreate(playerid)
{
    CharacterCreateFirst[playerid][0] = '\0';
    CharacterCreateLast[playerid][0] = '\0';
    CharacterCreateAge[playerid] = 18;
    CharacterCreateGender[playerid] = CHARACTER_GENDER_MALE;
    CharacterCreateOrigin[playerid][0] = '\0';
    CharacterCreateBackground[playerid][0] = '\0';
    return 1;
}

static bool:Character_IsNamePartValid(const name[])
{
    new len = strlen(name);

    if(!(2 <= len <= 12)) {
        return false;
    }

    for(new i; i < len; i++) {
        if(!((name[i] >= 'A' && name[i] <= 'Z') || (name[i] >= 'a' && name[i] <= 'z'))) {
            return false;
        }
    }
    return true;
}

static bool:Character_ParseNameInput(const input[], username[], usernameLen, firstName[], firstLen, lastName[], lastLen)
{
    new
        len = strlen(input),
        underscore = strfind(input, "_");

    if(!(2 <= len < usernameLen)) {
        return false;
    }

    if(underscore == -1) {
        if(!Character_IsNamePartValid(input)) {
            return false;
        }

        strcpy(username, input, usernameLen);
        strcpy(firstName, input, firstLen);
        lastName[0] = '\0';
        return true;
    }

    if(underscore == 0 || underscore == len - 1 || strfind(input, "_", false, underscore + 1) != -1) {
        return false;
    }

    new
        firstPart[24],
        lastPart[24];

    strmid(firstPart, input, 0, underscore, sizeof(firstPart));
    strmid(lastPart, input, underscore + 1, len, sizeof(lastPart));

    if(!Character_IsNamePartValid(firstPart) || !Character_IsNamePartValid(lastPart)) {
        return false;
    }

    strcpy(username, input, usernameLen);
    strcpy(firstName, firstPart, firstLen);
    strcpy(lastName, lastPart, lastLen);
    return true;
}

static bool:Character_IsValidSkin(skinid)
{
    return (0 <= skinid <= 311 && skinid != 74);
}

static Character_GetGenderLabel(gender, output[], len)
{
    switch(gender) {
        case CHARACTER_GENDER_FEMALE: format(output, len, "Nu");
        default: format(output, len, "Nam");
    }
    return 1;
}

static Character_BuildUsername(const firstName[], const lastName[], output[], len)
{
    if(isnull(lastName)) {
        strcpy(output, firstName, len);
    }
    else {
        format(output, len, "%s_%s", firstName, lastName);
    }
    return 1;
}

static Character_BuildDisplayName(const username[], const firstName[], const lastName[], output[], len)
{
    if(isnull(firstName)) {
        strcpy(output, username, len);
    }
    else if(isnull(lastName)) {
        strcpy(output, firstName, len);
    }
    else {
        format(output, len, "%s %s", firstName, lastName);
    }
    return 1;
}

static Character_EnsureStorage(characterid, skinid)
{
    new query[160];

    mysql_format(
        MySQL_GetHandle(),
        query,
        sizeof(query),
        "INSERT IGNORE INTO player_stats (u_id, skin, class_id, level) VALUES (%i, %i, %i, 1)",
        characterid,
        skinid,
        TEAM_CIVILIAN
    );
    mysql_tquery(MySQL_GetHandle(), query);

    mysql_format(MySQL_GetHandle(), query, sizeof(query), "INSERT IGNORE INTO player_items (u_id) VALUES (%i)", characterid);
    mysql_tquery(MySQL_GetHandle(), query);

    mysql_format(MySQL_GetHandle(), query, sizeof(query), "INSERT IGNORE INTO player_shots_stats (uid) VALUES (%i)", characterid);
    mysql_tquery(MySQL_GetHandle(), query);

    mysql_format(MySQL_GetHandle(), query, sizeof(query), "INSERT IGNORE INTO player_bank (u_id, money) VALUES (%i, 0)", characterid);
    mysql_tquery(MySQL_GetHandle(), query);
    return 1;
}

static bool:Character_IsOnline(characterid, exceptid = INVALID_PLAYER_ID)
{
    foreach(new i : Player) {
        if(i == exceptid) {
            continue;
        }

        if(Player_GetCharacterID(i) == characterid) {
            return true;
        }
    }
    return false;
}

static Character_StartSession(playerid, characterid, const username[], const firstName[], const lastName[], age, gender, const origin[], const background[], skinid)
{
    if(Character_IsOnline(characterid, playerid)) {
        SendErrorMsg(playerid, "Nhan vat nay dang online.");
        Account_ShowCharacterMenu(playerid);
        return 0;
    }

    Player_SetCharacterID(playerid, characterid);
    CharacterID[playerid] = characterid;
    CharacterAge[playerid] = age;
    CharacterGender[playerid] = gender;
    strcpy(CharacterUsername[playerid], username, sizeof(CharacterUsername[]));
    strcpy(CharacterFirstName[playerid], firstName, sizeof(CharacterFirstName[]));
    strcpy(CharacterLastName[playerid], lastName, sizeof(CharacterLastName[]));
    if(isnull(origin)) {
        strcpy(CharacterOrigin[playerid], "Chua cap nhat", sizeof(CharacterOrigin[]));
    }
    else {
        strcpy(CharacterOrigin[playerid], origin, sizeof(CharacterOrigin[]));
    }

    if(isnull(background)) {
        strcpy(CharacterBackground[playerid], "Chua cap nhat", sizeof(CharacterBackground[]));
    }
    else {
        strcpy(CharacterBackground[playerid], background, sizeof(CharacterBackground[]));
    }

    SetPlayerName(playerid, username);
    Player_SetSkin(playerid, skinid);
    Player_SetClass(playerid, TEAM_CIVILIAN);
    ResetPlayerWeapons(playerid);
    Player_SetLoginStatus(playerid, true);

    new displayName[MAX_PLAYER_NAME + 8];
    Character_BuildDisplayName(username, firstName, lastName, displayName, sizeof(displayName));

    SendServerMsgF(playerid, "Da vao nhan vat %s.", displayName);
    Character_EnsureStorage(characterid, skinid);
    CallLocalFunction("OnPlayerLogin", "i", playerid);
    return 1;
}

static Character_LoadAndStart(playerid, characterid)
{
    new query[512];

    inline const OnCharacterLoad()
    {
        if(!cache_num_rows()) {
            SendErrorMsg(playerid, "Khong tim thay nhan vat nay trong tai khoan cua ban.");
            Account_ShowCharacterMenu(playerid);
            return 1;
        }

        new
            username[MAX_PLAYER_NAME],
            firstName[24],
            lastName[24],
            origin[32],
            background[96],
            age,
            gender,
            skinid;

        cache_get_value_name(0, "username", username, sizeof(username));
        cache_get_value_name(0, "firstname", firstName, sizeof(firstName));
        cache_get_value_name(0, "lastname", lastName, sizeof(lastName));
        cache_get_value_name(0, "origin", origin, sizeof(origin));
        cache_get_value_name(0, "background", background, sizeof(background));
        cache_get_value_name_int(0, "age", age);
        cache_get_value_name_int(0, "gender", gender);
        cache_get_value_name_int(0, "skin", skinid);

        if(!Character_IsValidSkin(skinid)) {
            skinid = 1;
        }

        Character_StartSession(playerid, characterid, username, firstName, lastName, age, gender, origin, background, skinid);
        return 1;
    }

    if(characterid == Player_GetMasterAccountID(playerid)) {
        mysql_format(
            MySQL_GetHandle(),
            query,
            sizeof(query),
            "\
                SELECT \
                    p.username, p.firstname, p.lastname, p.age, p.gender, p.origin, p.background, \
                    COALESCE(ps.skin, 1) AS skin \
                FROM players AS p \
                LEFT JOIN player_stats AS ps ON ps.u_id = p.u_id \
                WHERE p.u_id = %i AND p.is_character = 0 \
                LIMIT 1",
            characterid
        );
    }
    else {
        mysql_format(
            MySQL_GetHandle(),
            query,
            sizeof(query),
            "\
                SELECT \
                    p.username, p.firstname, p.lastname, p.age, p.gender, p.origin, p.background, \
                    COALESCE(ps.skin, 1) AS skin \
                FROM players AS p \
                LEFT JOIN player_stats AS ps ON ps.u_id = p.u_id \
                WHERE p.u_id = %i AND p.account_id = %i AND p.is_character = 1 \
                LIMIT 1",
            characterid,
            Player_GetMasterAccountID(playerid)
        );
    }
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnCharacterLoad, query);
    return 1;
}

static Character_Insert(playerid, skinid)
{
    new
        username[MAX_PLAYER_NAME],
        firstName[24],
        lastName[24],
        origin[32],
        background[96],
        accountid = Player_GetMasterAccountID(playerid);

    strcpy(firstName, CharacterCreateFirst[playerid], sizeof(firstName));
    strcpy(lastName, CharacterCreateLast[playerid], sizeof(lastName));
    strcpy(origin, CharacterCreateOrigin[playerid], sizeof(origin));
    strcpy(background, CharacterCreateBackground[playerid], sizeof(background));
    Character_BuildUsername(firstName, lastName, username, sizeof(username));

    inline const OnCharacterNameCheck()
    {
        if(cache_num_rows()) {
            SendErrorMsg(playerid, "Ten nhan vat nay da ton tai.");
            Character_ShowFirstNameDialog(playerid);
            return 1;
        }

        inline const OnCharacterInserted()
        {
            new characterid = cache_insert_id();

            if(characterid <= 0) {
                SendErrorMsg(playerid, "Khong the tao nhan vat, vui long thu lai.");
                Account_ShowCharacterMenu(playerid);
                return 1;
            }

            Player_SetCharacterID(playerid, characterid);
            Player_SetSkin(playerid, skinid);
            CallLocalFunction("OnPlayerRegister", "i", playerid);
            Character_StartSession(
                playerid,
                characterid,
                username,
                firstName,
                lastName,
                CharacterCreateAge[playerid],
                CharacterCreateGender[playerid],
                origin,
                background,
                skinid
            );
            return 1;
        }

        new insertQuery[512];
        mysql_format(
            MySQL_GetHandle(),
            insertQuery,
            sizeof(insertQuery),
            "\
                INSERT INTO players \
                    (account_id, is_character, username, password, firstname, lastname, age, gender, origin, background) \
                VALUES \
                    (%i, 1, '%e', '"CHARACTER_PASSWORD_MARKER"', '%e', '%e', %i, %i, '%e', '%e')",
            accountid,
            username,
            firstName,
            lastName,
            CharacterCreateAge[playerid],
            CharacterCreateGender[playerid],
            origin,
            background
        );
        MySQL_TQueryInline(MySQL_GetHandle(), using inline OnCharacterInserted, insertQuery);
        return 1;
    }

    MySQL_TQueryInline(
        MySQL_GetHandle(),
        using inline OnCharacterNameCheck,
        "SELECT u_id FROM players WHERE username = '%e' LIMIT 1",
        username
    );
    return 1;
}

static Character_ShowSkinDialog(playerid)
{
    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused listitem

        if(!response) {
            Account_ShowCharacterMenu(playerid);
            return 1;
        }

        if(!IsNumeric(inputtext)) {
            SendErrorMsg(playerid, "Skin phai la so.");
            Character_ShowSkinDialog(playerid);
            return 1;
        }

        new skinid = strval(inputtext);

        if(!Character_IsValidSkin(skinid)) {
            SendErrorMsg(playerid, "Skin hop le la 0-311, tru skin 74.");
            Character_ShowSkinDialog(playerid);
            return 1;
        }

        Character_Insert(playerid, skinid);
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_INPUT,
        "Tao nhan vat - Skin",
        "Nhap ID skin muon dung.\nVi du: 1, 7, 23, 29, 46, 60, 170, 240.",
        "Tao",
        "Huy"
    );
    return 1;
}

static Character_ShowBackgroundDialog(playerid)
{
    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused listitem

        if(!response) {
            Account_ShowCharacterMenu(playerid);
            return 1;
        }

        if(strlen(inputtext) > 95) {
            SendErrorMsg(playerid, "Mo ta qua dai, toi da 95 ky tu.");
            Character_ShowBackgroundDialog(playerid);
            return 1;
        }

        if(isnull(inputtext)) {
            strcpy(CharacterCreateBackground[playerid], "Chua cap nhat", sizeof(CharacterCreateBackground[]));
        }
        else {
            strcpy(CharacterCreateBackground[playerid], inputtext, sizeof(CharacterCreateBackground[]));
        }
        Character_ShowSkinDialog(playerid);
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_INPUT,
        "Tao nhan vat - Mo ta",
        "Nhap mo ta ngan ve nhan vat.\nCo the de trong neu chua muon viet.",
        "Tiep",
        "Huy"
    );
    return 1;
}

static Character_ShowOriginDialog(playerid)
{
    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused listitem

        if(!response) {
            Account_ShowCharacterMenu(playerid);
            return 1;
        }

        new len = strlen(inputtext);

        if(!(3 <= len <= 31)) {
            SendErrorMsg(playerid, "Que quan phai tu 3-31 ky tu.");
            Character_ShowOriginDialog(playerid);
            return 1;
        }

        strcpy(CharacterCreateOrigin[playerid], inputtext, sizeof(CharacterCreateOrigin[]));
        Character_ShowBackgroundDialog(playerid);
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_INPUT,
        "Tao nhan vat - Que quan",
        "Nhap que quan/noi sinh cua nhan vat.",
        "Tiep",
        "Huy"
    );
    return 1;
}

static Character_ShowGenderDialog(playerid)
{
    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext

        if(!response) {
            Account_ShowCharacterMenu(playerid);
            return 1;
        }

        CharacterCreateGender[playerid] = listitem == 1 ? CHARACTER_GENDER_FEMALE : CHARACTER_GENDER_MALE;
        Character_ShowOriginDialog(playerid);
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        "Tao nhan vat - Gioi tinh",
        "Nam\nNu",
        "Chon",
        "Huy"
    );
    return 1;
}

static Character_ShowAgeDialog(playerid)
{
    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused listitem

        if(!response) {
            Account_ShowCharacterMenu(playerid);
            return 1;
        }

        if(!IsNumeric(inputtext)) {
            SendErrorMsg(playerid, "Tuoi phai la so.");
            Character_ShowAgeDialog(playerid);
            return 1;
        }

        new age = strval(inputtext);

        if(!(16 <= age <= 80)) {
            SendErrorMsg(playerid, "Tuoi hop le la 16-80.");
            Character_ShowAgeDialog(playerid);
            return 1;
        }

        CharacterCreateAge[playerid] = age;
        Character_ShowGenderDialog(playerid);
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_INPUT,
        "Tao nhan vat - Tuoi",
        "Nhap tuoi nhan vat.",
        "Tiep",
        "Huy"
    );
    return 1;
}

static Character_ShowFirstNameDialog(playerid)
{
    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused listitem

        if(!response) {
            Account_ShowCharacterMenu(playerid);
            return 1;
        }

        Character_ResetCreate(playerid);

        new username[MAX_PLAYER_NAME];

        if(!Character_ParseNameInput(
            inputtext,
            username,
            sizeof(username),
            CharacterCreateFirst[playerid],
            sizeof(CharacterCreateFirst[]),
            CharacterCreateLast[playerid],
            sizeof(CharacterCreateLast[])
        )) {
            SendErrorMsg(playerid, "Ten phai dung format Ho_Ten hoac Ten, chi dung chu cai khong dau.");
            Character_ShowFirstNameDialog(playerid);
            return 1;
        }

        Character_ShowAgeDialog(playerid);
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_INPUT,
        "Tao nhan vat - Ten",
        "Nhap ten nhan vat theo format Ho_Ten hoac Ten.\nVi du: Nguyen_Nam hoac Nam.",
        "Tiep",
        "Huy"
    );
    return 1;
}

stock Account_ShowCharacterMenu(playerid)
{
    if(Player_GetMasterAccountID(playerid) <= 0) {
        Kick(playerid);
        return 0;
    }

    new query[512];

    inline const OnCharacterListLoad()
    {
        new
            rows = cache_num_rows(),
            characterid,
            age,
            gender,
            skinid,
            level,
            username[MAX_PLAYER_NAME],
            firstName[24],
            lastName[24],
            origin[32],
            displayName[MAX_PLAYER_NAME + 8],
            genderName[8],
            line[144],
            list[768];

        CharacterMenuCount[playerid] = 1;
        CharacterMenuID[playerid][0] = Player_GetMasterAccountID(playerid);
        strcat(list, "Nhan vat\tThong tin\n", sizeof(list));
        strcat(list, "Nhan vat mac dinh\tDu lieu tai khoan cu\n", sizeof(list));

        for(new i; i < rows && CharacterMenuCount[playerid] < MAX_ACCOUNT_CHARACTERS; i++) {
            cache_get_value_name_int(i, "u_id", characterid);
            cache_get_value_name(i, "username", username, sizeof(username));
            cache_get_value_name(i, "firstname", firstName, sizeof(firstName));
            cache_get_value_name(i, "lastname", lastName, sizeof(lastName));
            cache_get_value_name(i, "origin", origin, sizeof(origin));
            cache_get_value_name_int(i, "age", age);
            cache_get_value_name_int(i, "gender", gender);
            cache_get_value_name_int(i, "skin", skinid);
            cache_get_value_name_int(i, "level", level);

            Character_GetGenderLabel(gender, genderName, sizeof(genderName));
            Character_BuildDisplayName(username, firstName, lastName, displayName, sizeof(displayName));
            CharacterMenuID[playerid][CharacterMenuCount[playerid]] = characterid;
            CharacterMenuCount[playerid] ++;

            format(
                line,
                sizeof(line),
                "%s\t%s, %d tuoi, %s, skin %d, level %d\n",
                displayName,
                genderName,
                age,
                origin,
                skinid,
                level
            );
            strcat(list, line, sizeof(list));
        }

        if(CharacterMenuCount[playerid] < MAX_ACCOUNT_CHARACTERS) {
            strcat(list, "Tao nhan vat moi\tTao slot nhan vat moi bang dialog\n", sizeof(list));
        }

        inline const _response(response, listitem, string:inputtext[])
        {
            #pragma unused inputtext

            if(!response) {
                Kick(playerid);
                return 1;
            }

            if(listitem < CharacterMenuCount[playerid]) {
                Character_LoadAndStart(playerid, CharacterMenuID[playerid][listitem]);
                return 1;
            }

            if(CharacterMenuCount[playerid] >= MAX_ACCOUNT_CHARACTERS) {
                SendErrorMsg(playerid, "Tai khoan nay da dat gioi han nhan vat.");
                Account_ShowCharacterMenu(playerid);
                return 1;
            }

            Character_ShowFirstNameDialog(playerid);
            return 1;
        }

        Dialog_ShowCallback(
            playerid,
            using inline _response,
            DIALOG_STYLE_TABLIST_HEADERS,
            "Chon nhan vat",
            list,
            "Chon",
            "Thoat"
        );
        return 1;
    }

    mysql_format(
        MySQL_GetHandle(),
        query,
        sizeof(query),
        "\
            SELECT \
                p.u_id, p.username, p.firstname, p.lastname, p.age, p.gender, p.origin, \
                COALESCE(ps.skin, 1) AS skin, COALESCE(ps.level, 1) AS level \
            FROM players AS p \
            LEFT JOIN player_stats AS ps ON ps.u_id = p.u_id \
            WHERE p.account_id = %i AND p.is_character = 1 \
            ORDER BY p.u_id ASC \
            LIMIT %i",
        Player_GetMasterAccountID(playerid),
        MAX_ACCOUNT_CHARACTERS - 1
    );
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnCharacterListLoad, query);
    return 1;
}

hook OnPlayerConnect(playerid)
{
    CharacterMenuCount[playerid] = 0;
    CharacterID[playerid] = 0;
    CharacterAge[playerid] = 0;
    CharacterGender[playerid] = CHARACTER_GENDER_MALE;
    CharacterUsername[playerid][0] = '\0';
    CharacterFirstName[playerid][0] = '\0';
    CharacterLastName[playerid][0] = '\0';
    CharacterOrigin[playerid][0] = '\0';
    CharacterBackground[playerid][0] = '\0';
    Character_ResetCreate(playerid);
    return 1;
}

stock Character_GetID(playerid)
{
    return CharacterID[playerid];
}

stock Character_GetFullName(playerid, output[], len)
{
    Character_BuildDisplayName(
        CharacterUsername[playerid],
        CharacterFirstName[playerid],
        CharacterLastName[playerid],
        output,
        len
    );
    return 1;
}

stock Character_GetAge(playerid)
{
    return CharacterAge[playerid];
}

stock Character_GetGender(playerid)
{
    return CharacterGender[playerid];
}

stock Character_GetOrigin(playerid, output[], len)
{
    strcpy(output, CharacterOrigin[playerid], len);
    return 1;
}

stock Character_GetBackground(playerid, output[], len)
{
    strcpy(output, CharacterBackground[playerid], len);
    return 1;
}

stock Character_GetGenderName(playerid, output[], len)
{
    Character_GetGenderLabel(CharacterGender[playerid], output, len);
    return 1;
}
