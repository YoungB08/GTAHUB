#define MAX_COMMANDS_PER_LEVEL  (50)

static
    aCommands[MAX_ADMIN_LEVELS][MAX_COMMANDS_PER_LEVEL][2048],
    Iterator:gAdminCommands[MAX_ADMIN_LEVELS]<MAX_COMMANDS_PER_LEVEL>
;

stock Admin_GetRankNameFromLevel(alevel)
{
    new admin_RankNames[6][24] =
    {
        "Khong co",      // 0
        "Admin Tap Su",  // 1
        "Admin Thuong",  // 2
        "Admin Cao Cap", // 3
        "Truong Admin",  // 4
        "Developer"      // 5
    };

    return admin_RankNames[alevel];
}

stock Admin_AddCommandToList(alevel, const string[])
{
    if (!(ADM_LVL_1 <= alevel <= ADM_LVL_5))
    {
        printf("LOI: Khong the dang ky lenh admin cho level %d", alevel);
        return 0;
    }

    new tmpString[152];
    new freeid = Iter_Alloc(gAdminCommands[alevel - 1]);

    if(freeid == ITER_NONE) {
        Logger_Err("DA DAT GIOI HAN LENH ADMIN MOI LEVEL",
            Logger_S("Lenh", string),
            Logger_I("Level", alevel));
        return ITER_NONE;
    }
    format(tmpString, sizeof tmpString, "%s\n", string);
    strcat(aCommands[alevel - 1][freeid], tmpString);
    return 1;
}

ACMD:[1]acmds(playerid, params[])
{
    Admin_ShowLevelList(playerid);
    return 1;
}

ACMD:[1]ah(playerid, params[])
{
    Admin_ShowLevelList(playerid);
    return 1;
}

ACMD:[1]ahelp(playerid, params[])
{
    Admin_ShowLevelList(playerid);
    return 1;
}

static Admin_ShowLevelList(playerid)
{
    new tmpString[152], ranks[256];
    new alevel = Admin_GetPlayerLevel(playerid);

    inline const _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, inputtext
        if(!response) return 1;
        Player_ShowAdminCmdsOfLevel(playerid, listitem);
        return 1;
    }

    for(new i; i < alevel; i++) {
        new newlevel = i + 1;
        format(tmpString, sizeof tmpString, "%s\n", Admin_GetRankNameFromLevel(newlevel));
        strcat(ranks, tmpString);
    }

    Dialog_ShowCallback(playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        "Lenh Admin",
        ranks,
        "Chon",
        "Dong");
    return 1;
}

static Player_ShowAdminCmdsOfLevel(playerid, alevel) {
    new acmds[2048], tmpString[50];

    inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, listitem, inputtext
        if(!response) {
            Admin_ShowLevelList(playerid);
            return 1;
        }
        return 1;
    }

    foreach(new i : gAdminCommands[alevel])
    {
        strcat(acmds, aCommands[alevel][i]);
    }

    format(tmpString, sizeof tmpString, "%s", Admin_GetRankNameFromLevel(alevel + 1));
    Dialog_ShowCallback(playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        tmpString,
        acmds,
        "Chon",
        "Quay lai");
    return 1;
}
