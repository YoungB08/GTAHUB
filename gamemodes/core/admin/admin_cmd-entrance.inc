#include <YSI_Coding\y_hooks>

enum E_ENTRANCE_PRESET_DATA {
    E_PRESET_NAME[MAX_STORE_NAME],
    E_PRESET_ICON,
    E_PRESET_INTERIOR,
    bool:E_PRESET_FREEZE,
    Float:E_PRESET_POS[4]
}

static const EntranceInteriorPresets[][E_ENTRANCE_PRESET_DATA] = {
    {"PHONG GYM", 54, 5, false, {772.3065, -5.5157, 1000.7285, 0.0}},
    {"QUAN BAR", 49, 11, false, {501.9013, -67.5635, 998.7578, 0.0}},
    {"CUA HANG GA RAN", 14, 9, false, {364.9896, -11.8441, 1001.8516, 0.0}},
    {"BINCO", 45, 15, false, {207.6674, -111.2659, 1005.1328, 0.0}},
    {"CUA HANG VU KHI", 6, 4, false, {285.8562, -86.7820, 1001.5229, 0.0}},
    {"CUA HANG VU KHI 2", 6, 6, false, {316.3490, -170.2974, 999.5938, 0.0}},
    {"CUA HANG NGUOI LON", 21, 3, false, {-100.3562, -25.0387, 1000.7188, 0.0}},
    {"TRAM XANG", 55, 6, false, {-27.2923, -58.0535, 1003.5469, 0.0}},
    {"CUA HANG BURGER", 10, 10, false, {362.8823, -75.1634, 1001.5078, 0.0}},
    {"TIEM TOC", 7, 12, false, {411.8917, -54.4434, 1001.8984, 0.0}},
    {"TRUNG TAM DICH VU", 30, 6, false, {246.8373, 62.3343, 1003.6406, 0.0}},
    {"TIEM PIZZA", 29, 5, false, {372.2738, -133.5248, 1001.4922, 0.0}},
    {"CAU LAC BO DEM", 36, 2, false, {1204.7625, -13.8523, 1000.9219, 0.0}},
    {"SAN NHAY", 48, 17, false, {493.4810, -24.9531, 1000.6719, 0.0}},
    {"24/7", 61, 17, false, {-25.9472, -188.2597, 1003.5469, 0.0}},
    {"TIEM XAM HINH", 39, 16, false, {-204.4170, -27.3470, 1002.2734, 0.0}},
    {"CUA HANG THOI TRANG", 45, 1, false, {203.8414, -50.6566, 1001.8047, 0.0}},
    {"CUA HANG THOI TRANG 2", 45, 5, false, {227.3678, -8.3722, 1002.2109, 0.0}},
    {"TIEM BANH", 17, 17, false, {377.1306, -193.3048, 1000.6328, 0.0}},
    {"TOA THI CHINH", 12, 3, false, {390.7462, 173.7627, 1008.3828, 0.0}},
    {"KHACH SAN JEFFERSON", 59, 15, false, {2214.3845, -1150.4780, 1025.7969, 0.0}},
    {"CUA HANG THE THAO", 45, 3, false, {207.0255, -140.3765, 1003.5078, 0.0}},
    {"CUA HANG THOI TRANG 3", 45, 18, false, {161.2982, -97.1033, 1001.8047, 0.0}},
    {"NGAN HANG", 52, 0, false, {2306.1572, -16.2888, 26.7496, 0.0}},
    {"SONG BAC", 25, 12, false, {1133.2094, -15.3630, 1000.6797, 0.0}},
    {"BIET THU", 26, 5, false, {1299.0386, -796.7242, 1084.0078, 0.0}},
    {"DAI LY XE", 55, 1, true, {849.4232, -973.6054, 1090.1021, 0.0}},
    {"NHA KHO", 24, 1, true, {742.9769, 207.1607, 11558.7939, 0.0}}
};

static Admin_ShowEntranceGotoDialog(playerid)
{
    new list[4096];

    if(!Entrance_BuildAdminList(playerid, list, sizeof(list))) {
        return SendErrorMsg(playerid, "Chua co entrance nao de goto.");
    }

    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        new entranceid = Entrance_GetAdminListSelection(playerid, listitem);

        if(!Entrance_TeleportAdmin(playerid, entranceid)) {
            return SendErrorMsg(playerid, "Entrance khong hop le.");
        }

        SendServerMsgF(playerid, "Da teleport den entrance id %i.", entranceid);
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_TABLIST_HEADERS, "Goto Entrance", list, "Den", "Dong");
    return 1;
}

static Admin_ShowEntranceActionDialog(playerid, entranceid)
{
    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        if(listitem == 0) {
            Entrance_TeleportAdmin(playerid, entranceid, false);
            return 1;
        }

        if(listitem == 1) {
            Entrance_TeleportAdmin(playerid, entranceid, true);
            return 1;
        }

        if(Admin_GetPlayerLevel(playerid) < ADM_LVL_5) {
            return SendErrorMsg(playerid, "Chi admin level 5 moi duoc sua/xoa entrance.");
        }

        if(listitem == 2 || listitem == 3) {
            new
                Float:x,
                Float:y,
                Float:z,
                Float:a;

            GetPlayerPos(playerid, x, y, z);
            GetPlayerFacingAngle(playerid, a);

            if(listitem == 2) {
                Entrance_SetEntryPosition(entranceid, x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
                SendServerMsgF(playerid, "Da dat lai vi tri loi vao cho entrance id %i.", entranceid);
                return 1;
            }

            Entrance_SetExitPosition(entranceid, x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid));
            SendServerMsgF(playerid, "Da dat lai vi tri loi ra cho entrance id %i.", entranceid);
            return 1;
        }

        if(listitem == 4) {
            inline const _rename(response2, listitem2, string:name[])
            {
                #pragma unused listitem2

                if(!response2) {
                    return 1;
                }

                if(isnull(name) || strlen(name) >= MAX_STORE_NAME) {
                    SendErrorMsgF(playerid, "Ten phai tu 1-%i ky tu.", MAX_STORE_NAME - 1);
                    return Admin_ShowEntranceActionDialog(playerid, entranceid);
                }

                Entrance_SetPairName(entranceid, name);
                SendServerMsgF(playerid, "Da doi ten entrance id %i thanh %s.", entranceid, name);
                return 1;
            }

            Dialog_ShowCallback(playerid, using inline _rename, DIALOG_STYLE_INPUT, "Doi ten Entrance", "Nhap ten moi.", "Luu", "Huy");
            return 1;
        }

        if(listitem == 5) {
            inline const _delete(response2, listitem2, string:confirm[])
            {
                #pragma unused listitem2
                #pragma unused confirm

                if(!response2) {
                    return 1;
                }

                Entrance_DeletePair(entranceid);
                SendServerMsgF(playerid, "Da xoa entrance id %i.", entranceid);
                return 1;
            }

            Dialog_ShowCallback(playerid, using inline _delete, DIALOG_STYLE_MSGBOX, "Xoa Entrance", "Xac nhan xoa entrance nay?", "Xoa", "Huy");
            return 1;
        }
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        "Quan ly Entrance",
        "Den loi vao\nDen loi ra\nDat loi vao tai vi tri hien tai\nDat loi ra tai vi tri hien tai\nDoi ten\nXoa",
        "Chon",
        "Dong"
    );
    return 1;
}

static Admin_ShowEntranceManageList(playerid)
{
    new list[4096];

    if(!Entrance_BuildAdminList(playerid, list, sizeof(list))) {
        return SendErrorMsg(playerid, "Chua co entrance nao de quan ly.");
    }

    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        new entranceid = Entrance_GetAdminListSelection(playerid, listitem);

        if(entranceid == INTERACTION_INVALID_ID) {
            return SendErrorMsg(playerid, "Entrance khong hop le.");
        }

        return Admin_ShowEntranceActionDialog(playerid, entranceid);
    }

    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_TABLIST_HEADERS, "Danh sach Entrance", list, "Chon", "Dong");
    return 1;
}

static Admin_ShowAddEntranceDialog(playerid)
{
    new list[2048];

    for(new i; i < sizeof(EntranceInteriorPresets); i++) {
        new line[96];
        format(
            line,
            sizeof(line),
            "%s\tInterior %i\n",
            EntranceInteriorPresets[i][E_PRESET_NAME],
            EntranceInteriorPresets[i][E_PRESET_INTERIOR]
        );
        strcat(list, line, sizeof(list));
    }

    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        if(!(0 <= listitem < sizeof(EntranceInteriorPresets))) {
            return SendErrorMsg(playerid, "Interior khong hop le.");
        }

        new
            Float:x,
            Float:y,
            Float:z,
            Float:a,
            world = GetPlayerVirtualWorld(playerid),
            interior = GetPlayerInterior(playerid),
            targetWorld = Entrance_AllocateVirtualWorld(),
            entryid;

        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, a);

        entryid = Entrance_CreatePair(
            EntranceInteriorPresets[listitem][E_PRESET_NAME],
            EntranceInteriorPresets[listitem][E_PRESET_ICON],
            EntranceInteriorPresets[listitem][E_PRESET_FREEZE],
            x,
            y,
            z,
            a,
            world,
            interior,
            EntranceInteriorPresets[listitem][E_PRESET_POS][0],
            EntranceInteriorPresets[listitem][E_PRESET_POS][1],
            EntranceInteriorPresets[listitem][E_PRESET_POS][2],
            EntranceInteriorPresets[listitem][E_PRESET_POS][3],
            targetWorld,
            EntranceInteriorPresets[listitem][E_PRESET_INTERIOR]
        );

        if(entryid == INTERACTION_INVALID_ID) {
            return SendErrorMsg(playerid, "Khong the tao entrance, da dat gioi han?");
        }

        Entrance_SavePairToDatabase(entryid);
        SendServerMsgF(playerid, "Da tao va luu SQL entrance id %i.", entryid);
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_TABLIST_HEADERS, "Them Entrance", list, "Tao", "Dong");
    return 1;
}

ACMD:[2]gotoentrance(playerid, params[])
{
    #pragma unused params
    return Admin_ShowEntranceGotoDialog(playerid);
}

ALTCMD:gotodoor(gotoentrance);
ALTCMD:gotobiz(gotoentrance);

ACMD:[2]entrances(playerid, params[])
{
    #pragma unused params
    return Admin_ShowEntranceManageList(playerid);
}

ALTCMD:entrance(entrances);

ACMD:[5]addentrance(playerid, params[])
{
    #pragma unused params
    return Admin_ShowAddEntranceDialog(playerid);
}

ACMD:[5]createentrance(playerid, params[])
{
    new
        Float:to_x,
        Float:to_y,
        Float:to_z,
        Float:to_ang,
        to_world,
        to_interior,
        bool:freeze;

    if(sscanf(params, "p<,>ffffiil", to_x, to_y, to_z, to_ang, to_world, to_interior, freeze)) {
        return SendSyntaxMsg(playerid, "/createentrance x,y,z,angle,world,interior,freeze(0/1)");
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:ang,
        interior = GetPlayerInterior(playerid),
        world = GetPlayerVirtualWorld(playerid),
        entryid;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, ang);

    entryid = Entrance_CreatePair(
        "LOI VAO",
        0,
        freeze,
        x,
        y,
        z,
        ang,
        world,
        interior,
        to_x,
        to_y,
        to_z,
        to_ang,
        to_world,
        to_interior
    );

    if(entryid == INTERACTION_INVALID_ID) {
        return SendErrorMsg(playerid, "Khong the tao entrance, da dat gioi han?");
    }

    Entrance_SavePairToDatabase(entryid);
    SendServerMsgF(playerid, "Da tao va luu SQL entrance id %i.", entryid);
    return 1;
}

ACMD:[5]editentrance(playerid, params[])
{
    new entranceid;

    if(sscanf(params, "I(-1)", entranceid)) {
        return Admin_ShowEntranceManageList(playerid);
    }

    if(entranceid == -1) {
        return Admin_ShowEntranceManageList(playerid);
    }

    return Admin_ShowEntranceActionDialog(playerid, entranceid);
}

ACMD:[5]deleteentrance(playerid, params[])
{
    new entranceid;

    if(sscanf(params, "i", entranceid)) {
        return SendSyntaxMsg(playerid, "/deleteentrance [local entrance id]");
    }

    if(!Entrance_DeletePair(entranceid)) {
        return SendErrorMsg(playerid, "Entrance khong hop le.");
    }

    SendServerMsgF(playerid, "Da xoa entrance id %i.", entranceid);
    return 1;
}

ACMD:[5]renameentrance(playerid, params[])
{
    new
        entranceid,
        name[MAX_STORE_NAME];

    if(sscanf(params, "is[30]", entranceid, name)) {
        return SendSyntaxMsg(playerid, "/renameentrance [local entrance id] [ten]");
    }

    if(!Entrance_SetPairName(entranceid, name)) {
        return SendErrorMsg(playerid, "Entrance khong hop le.");
    }

    SendServerMsgF(playerid, "Da doi ten entrance id %i thanh %s.", entranceid, name);
    return 1;
}

ACMD:[5]setentranceentry(playerid, params[])
{
    new entranceid;

    if(sscanf(params, "i", entranceid)) {
        return SendSyntaxMsg(playerid, "/setentranceentry [local entrance id]");
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    if(!Entrance_SetEntryPosition(entranceid, x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid))) {
        return SendErrorMsg(playerid, "Entrance khong hop le.");
    }

    SendServerMsgF(playerid, "Da dat loi vao moi cho entrance id %i.", entranceid);
    return 1;
}

ACMD:[5]setentranceexit(playerid, params[])
{
    new entranceid;

    if(sscanf(params, "i", entranceid)) {
        return SendSyntaxMsg(playerid, "/setentranceexit [local entrance id]");
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    if(!Entrance_SetExitPosition(entranceid, x, y, z, a, GetPlayerVirtualWorld(playerid), GetPlayerInterior(playerid))) {
        return SendErrorMsg(playerid, "Entrance khong hop le.");
    }

    SendServerMsgF(playerid, "Da dat loi ra moi cho entrance id %i.", entranceid);
    return 1;
}
