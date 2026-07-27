#include <YSI_Coding\y_hooks>

const
    Float:gBombShopX = 1857.4403,
    Float:gBombShopY = -1852.6626,
    Float:gBombShopZ = 13.5815,
    Float:gBombShopA = 264.0457;

const
    C4_PRICE = 3_000;

static
    gBombDealerAreaID;

hook OnGameModeInit() {
    gBombDealerAreaID = CreateDynamicCircle(
        Float:gBombShopX,
        Float:gBombShopY,
        Float:3.5);

    CreateDynamicActor(28,
        gBombShopX,
        gBombShopY,
        gBombShopZ,
        gBombShopA
    );

    CreateDynamic3DTextLabel(
        "Nguoi ban bom\n\
        Pablo\n\
        Nhan "C_YELLOW"~k~~CONVERSATION_YES~ "C_ORANGE"de tuong tac",
        COLOR_ORANGE,
        gBombShopX,
        gBombShopY,
        gBombShopZ + 1.5,
        50.0,
        .testlos = 1
    );

    CreateDynamicMapIcon(
        gBombShopX,
        gBombShopY,
        gBombShopZ,
        8,
        -1,
        .streamdistance = 200.0,
        .style = MAPICON_GLOBAL
    );

    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(PRESSED(KEY_YES) && IsPlayerInDynamicArea(playerid, gBombDealerAreaID)) {
        _BombUI_Show(playerid);
    }
    return 1;
}

static void:_BombUI_Show(playerid) {

    new
        limit = (MAX_ITEM - Player_GetC4(playerid));

    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused listitem
        if(!response) {
            return;
        }

        new
            amount = strval(inputtext),
            price = (C4_PRICE * amount);

        if(amount <= 0) {
            SendErrorMsg(playerid, "So luong khong hop le.");
            _BombUI_Show(playerid);
            return;
        }

        if(amount > limit) {
            SendErrorMsg(playerid, "So luong vuot qua gioi han cua ban.");
            _BombUI_Show(playerid);
            return;
        }

        if(GetPlayerMoney(playerid) < price) {
            SendErrorMsgF(playerid, "Ban khong du tien de mua "C_GREY"%i "C_WHITE"C4", amount);
            _BombUI_Show(playerid);
            return;
        }

        GivePlayerMoney(playerid, -price);
        Player_GiveC4(playerid, amount);

        SendMsgF(playerid, COLOR_ORANGE, "(nguoi ban bom) "C_WHITE"Cam on da mua "C_GREY"%i "C_WHITE"thuoc no C4", amount);
    }

    new
        string[128 + 32];

    format(
        string,
        sizeof(string),
        "\
        "C_WHITE"Ban dang co "C_YELLOW"%i C4\n\n\
        "C_WHITE"Nhap so luong C4 ban muon mua.\n\n\
        "C_GREEN"GOI Y!\n\
        "C_WHITE"Ban co the mua toi da "C_YELLOW"%i C4\
        ",
        Player_GetC4(playerid),
        limit
    );

    Dialog_ShowCallback(playerid,
        using inline _response,
        DIALOG_STYLE_INPUT,
        "Cua hang bom",
        string,
        "Mua",
        "Dong"
    );



}
