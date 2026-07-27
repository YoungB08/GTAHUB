#include <houses_empty-interior>
#include <houses_interior-data>
#include <houses_impl>
#include <houses_entext-handler>
#include <houses_cmd>
#include <houses_lock>
#include <houses_key_holder>

#include <furniture>

#include <YSI_Coding\y_hooks>

enum E_QUOTE_DATA {
    E_QUOTE_CONTENT,
    E_QUOTE_AUTHOR
}

// Thanks to https://www.quoteambition.com/home-quotes/
// ligthen the player's mood with nice messages once
// they have entered their house.
static const HomeQuotes[][E_QUOTE_DATA][] = {
    {"Nha la noi ta lam viec, va ta lam viec o khap moi noi.", "Alfred Nobel"},
    {"Mot ngoi nha khong co sach giong nhu co the khong co linh hon.", "Marcus Tullius Cicero"},
    {"Nha la noi che cho khoi moi con bao.", "William J. Bennett"},
    {"Nha la tat ca nhung gi ban co the buoc toi.", "Jerry Spinelli"},
    {"Toi muon nha minh la noi tiep suc, moi goi va chao don.", "Mary DeMuth"},
    {"Ho tao ra mot mai nha noi toi thay an toan va co the mac sai lam.", "Sarah Williams"},
    {"Hay danh thoi gian cuoi tuan cai thien nha cua ban va thai do voi gia dinh.", "Bo Bennett"},
    {"Nha la noi ban cam thay thuoc ve va duoc doi xu tot.", "Dalai Lama"},
    {"Noi ban dang o, do la nha.", "Emily Dickinson"},
    {"Nha la noi moi thu bat dau.", "T. S. Eliot"}
};

static
    BitArray: PlayerInsideAHouse<MAX_PLAYERS>;

stock Player_IsInsideHouse(playerid) {
    return Bit_Get(PlayerInsideAHouse, playerid);
}

hook OnPlayerConnect(playerid) {
    Bit_Set(PlayerInsideAHouse, playerid, false);
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(PRESSED(KEY_NO) && Interaction_IsActive(playerid, INTERACTION_TYPE_HOUSE)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_HOUSE, Interaction_GetActiveID(playerid));
        return 1;
    }

    if(PRESSED(KEY_NO) && Player_IsInsideHouse(playerid)) {
        return CommandExecute(playerid, "furniture");
    }

    if(PRESSED(KEY_YES) || PRESSED(KEY_SECONDARY_ATTACK)) {
        new
            houseid = Player_GetInHouseID(playerid);
        if(Player_InHouseEntrance(playerid, houseid)) {
            if(
                (House_PlayerIsOwner(playerid, houseid) && House_IsLocked(houseid))
                || !House_IsLocked(houseid)
            ) {

                new
                    homeMessage[256];

                new
                    idx = clamp(random(sizeof(HomeQuotes)), 0, sizeof(HomeQuotes));

                strcat(homeMessage, "\"");
                strcat(homeMessage, HomeQuotes[idx][E_QUOTE_CONTENT]);
                strcat(homeMessage, "\"");

                strcat(homeMessage, "~n~");
                strcat(homeMessage, "-");
                strcat(homeMessage, " ");
                strcat(homeMessage, "~y~");
                strcat(homeMessage, HomeQuotes[idx][E_QUOTE_AUTHOR]);

                strcat(homeMessage, "~n~~n~");
                strcat(homeMessage, "~w~Ban co the nhan ~r~N ~w~de mo menu noi that.");

                MessageBox_Show(
                    playerid,
                    TYPE_MSGBOX,
                    "~g~Chao mung ve nha!",
                    homeMessage
                );

                new
                    Float: x,
                    Float: y,
                    Float: z,
                    interiorid,
                    bool:unused;

                House_GetInteriorData(
                    House_GetInterior(houseid),
                    x,
                    y,
                    z,
                    interiorid,
                    unused,
                    unused
                );

                Interaction_ClearPrompt(playerid, INTERACTION_TYPE_HOUSE, houseid);
                Interaction_LoadSafeTeleport(playerid, x, y, z, 0.0, House_GetWorld(houseid), interiorid, true);

                Bit_Set(PlayerInsideAHouse, playerid, true);
            }
        }

        if(Player_InHouseExit(playerid, houseid)) {

            if(!(Player_IsInsideHouse(playerid))) {
                return 1;
            }

            if (Player_IsOnFurnitureMode(playerid)) {
                return 1;
            }

            MessageBox_Show(
                playerid,
                TYPE_MSGBOX,
                "~r~Tam biet",
                "Hen gap lai."
            );

            new
                Float: x,
                Float: y,
                Float: z,
                Float: a;

            House_GetEntranceData(houseid, x, y, z, a);

            Interaction_ClearPrompt(playerid, INTERACTION_TYPE_HOUSE, houseid);
            Interaction_LoadSafeTeleport(playerid, x, y, z, a, 0, 0, true);

            Bit_Set(PlayerInsideAHouse, playerid, false);
        }
    }
    return 1;
}
