/*
    Written by s4marium

    todo:
    - need to change the configs for the actors locations its with the same as textlabels
    - set the ideal prices on each items
 */
#include <YSI_Coding\y_hooks>

// ==================== CONFIGURATION ====================

#define SHOP_BUYER_X        2123.0
#define SHOP_BUYER_Y        2492.0
#define SHOP_BUYER_Z        10.8
#define SHOP_BUYER_FACING   180.0
#define SHOP_BUYER_SKIN     15

#define SHOP_SELLER_X       2145.0
#define SHOP_SELLER_Y       2492.0
#define SHOP_SELLER_Z       10.8
#define SHOP_SELLER_FACING  0.0
#define SHOP_SELLER_SKIN    49

#define BUYER_NAME          "Farm Supplier"
#define SELLER_NAME         "Harvest Master"

// ==================== BUY ITEMS ====================

#define MAX_BUY_ITEMS 2

new const BuyNames[][32] = {
    "Fruit Picker",
    "Empty Bottles"
};

new const BuyPrices[MAX_BUY_ITEMS] = {
    2500,
    75
};

// ==================== SELL ITEMS ====================

#define MAX_SELL_ITEMS 2

new const SellNames[][32] = {
    "Orange",
    "Orange Juice"
};

new const SellPrices[MAX_SELL_ITEMS] = {
    400,
    1200
};

// ==================== DIALOG IDS ====================

#define DIALOG_FARM_MAIN_MENU           2901
#define DIALOG_FARM_BUY_MENU            2902
#define DIALOG_FARM_SELL_MENU           2903
#define DIALOG_FARM_BUY_CONFIRM         2904
#define DIALOG_FARM_SELL_CONFIRM        2905
#define DIALOG_FARM_INVENTORY           2906
#define DIALOG_FARM_BUY_BOTTLES_INPUT   2907
#define DIALOG_FARM_SELL_QUANTITY_INPUT 2908
#define DIALOG_FARM_BUY_QUANTITY_INPUT  2909

// ==================== NPC ACTOR IDS ====================

new buyerActorID;
new sellerActorID;

// ==================== INITIALIZATION ====================

hook OnGameModeInit() {
    buyerActorID = CreateActor(SHOP_BUYER_SKIN, SHOP_BUYER_X, SHOP_BUYER_Y, SHOP_BUYER_Z, SHOP_BUYER_FACING);
    SetActorVirtualWorld(buyerActorID, 0);
    SetActorInvulnerable(buyerActorID, true);
    ApplyActorAnimation(buyerActorID, "SHOP", "ROB_Shifty", 4.1, true, false, false, false, 1);
    CreateDynamic3DTextLabel("Farm Supplier\n"C_WHITE"[/buyfarm]", COLOR_GREEN, SHOP_BUYER_X, SHOP_BUYER_Y, SHOP_BUYER_Z + 1.0, 15.0);

    sellerActorID = CreateActor(SHOP_SELLER_SKIN, SHOP_SELLER_X, SHOP_SELLER_Y, SHOP_SELLER_Z, SHOP_SELLER_FACING);
    SetActorVirtualWorld(sellerActorID, 0);
    SetActorInvulnerable(sellerActorID, true);
    ApplyActorAnimation(sellerActorID, "SHOP", "ROB_Shifty", 4.1, true, false, false, false, 1);
    CreateDynamic3DTextLabel("Harvest Master\n"C_WHITE"[/sellfarm]", COLOR_YELLOW, SHOP_SELLER_X, SHOP_SELLER_Y, SHOP_SELLER_Z + 1.0, 15.0);

    print("[Farmers Market] System loaded");
    return 1;
}

// ==================== DIALOGS ====================

ShowFarmMainMenu(playerid, npcType) {
    new dialogTitle[64];
    if (npcType == 0) {
        format(dialogTitle, sizeof(dialogTitle), "{00AA00}%s{FFFFFF} - Farm Supplies", BUYER_NAME);
        ShowPlayerDialog(playerid, DIALOG_FARM_MAIN_MENU, DIALOG_STYLE_LIST, dialogTitle,
            "{00AA00}Buy Farm Tools\n{FF0000}Leave", "Select", "Cancel");
        SetPVarInt(playerid, "FarmNPCType", 0);
    } else {
        format(dialogTitle, sizeof(dialogTitle), "{FFAA00}%s{FFFFFF} - Harvest Sales", SELLER_NAME);
        ShowPlayerDialog(playerid, DIALOG_FARM_MAIN_MENU, DIALOG_STYLE_LIST, dialogTitle,
            "{00AA00}Sell Harvest\n{FF0000}Leave", "Select", "Cancel");
        SetPVarInt(playerid, "FarmNPCType", 1);
    }
    return 1;
}

ShowBuyMenu(playerid) {
    new itemList[256], optionText[96];

    new fruitPickerCount = Player_GetFruitPicker(playerid);
    new bottlesCount = Player_GetBottles(playerid);

    format(optionText, sizeof(optionText), "{00AA00}%s {FFFFFF}(Have: %d/1) - ${FFFF00}%d\n",
        BuyNames[0], fruitPickerCount, BuyPrices[0]);
    itemList = optionText;

    format(optionText, sizeof(optionText), "{00AA00}%s {FFFFFF}(Have: %d/25) - ${FFFF00}%d/ea\n",
        BuyNames[1], bottlesCount, BuyPrices[1]);
    strcat(itemList, optionText);

    strcat(itemList, "\n{FF0000}Back");

    ShowPlayerDialog(playerid, DIALOG_FARM_BUY_MENU, DIALOG_STYLE_LIST,
        "{00AA00}[Farm Supplier] What to buy?", itemList, "Select", "Cancel");
    return 1;
}

ShowSellMenu(playerid) {
    new itemList[256], optionText[96];

    for (new i = 0; i < MAX_SELL_ITEMS; i++) {
        new itemCount = 0;
        if (i == 0) itemCount = Player_GetOranges(playerid);
        else if (i == 1) itemCount = Player_GetOrangeJuice(playerid);

        format(optionText, sizeof(optionText), "{FFAA00}%s {FFFFFF}(Have: %d) - ${FFFF00}%d/ea\n",
            SellNames[i], itemCount, SellPrices[i]);
        if (i == 0) itemList = optionText;
        else strcat(itemList, optionText);
    }

    strcat(itemList, "\n{FF0000}Back");

    ShowPlayerDialog(playerid, DIALOG_FARM_SELL_MENU, DIALOG_STYLE_LIST,
        "{FFAA00}[Harvest Master] What to sell?", itemList, "Select", "Cancel");
    return 1;
}

// ==================== DIALOG HANDLER ====================

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]) {
    switch(dialogid) {
        case DIALOG_FARM_MAIN_MENU: {
            if (!response) return 0;
            new npcType = GetPVarInt(playerid, "FarmNPCType");
            if (npcType == 0) {
                if (listitem == 0) ShowBuyMenu(playerid);
            } else {
                if (listitem == 0) ShowSellMenu(playerid);
            }
            return 1;
        }

        case DIALOG_FARM_BUY_MENU: {
            if (!response) {
                ShowFarmMainMenu(playerid, 0);
                return 1;
            }
            if (listitem == 2) {
                ShowFarmMainMenu(playerid, 0);
                return 1;
            }

            if (listitem == 0) {
                new fruitCount = Player_GetFruitPicker(playerid);
                if (fruitCount >= 1) {
                    Alert_Send(playerid, "~g~[Farm Supplier]~r~ You already have maximum pickers (1/1)!", 3000);
                    ShowBuyMenu(playerid);
                    return 1;
                }
                SetPVarInt(playerid, "BuyItem", 0);
                new inputMsg[128];
                format(inputMsg, sizeof(inputMsg), "{00AA00}%s\n{FFFFFF}Price: ${FFFF00}%d\n{00AA00}You have: {FFFFFF}%d/1\n{00AA00}How many to buy?", BuyNames[0], BuyPrices[0], fruitCount);
                ShowPlayerDialog(playerid, DIALOG_FARM_BUY_QUANTITY_INPUT, DIALOG_STYLE_INPUT,
                    "{00AA00}Buy Item", inputMsg, "Buy", "Cancel");
                return 1;
            }

            if (listitem == 1) {
                new bottleCount = Player_GetBottles(playerid);
                SetPVarInt(playerid, "BuyItem", 1);
                new inputMsg[128];
                format(inputMsg, sizeof(inputMsg), "{00AA00}%s\n{FFFFFF}Price: ${FFFF00}%d/ea\n{00AA00}You have: {FFFFFF}%d/25\n{00AA00}How many to buy? (1-25)", BuyNames[1], BuyPrices[1], bottleCount);
                ShowPlayerDialog(playerid, DIALOG_FARM_BUY_QUANTITY_INPUT, DIALOG_STYLE_INPUT,
                    "{00AA00}Buy Item", inputMsg, "Buy", "Cancel");
                return 1;
            }
            return 1;
        }

        case DIALOG_FARM_BUY_QUANTITY_INPUT: {
            if (!response) {
                ShowBuyMenu(playerid);
                return 1;
            }
            new qty = strval(inputtext);
            new item = GetPVarInt(playerid, "BuyItem");

            if (item == 0) {
                if (qty != 1) {
                    Alert_Send(playerid, "~g~[Farm Supplier]~r~ You can only buy 1 picker!", 3000);
                    ShowBuyMenu(playerid);
                    return 1;
                }
                if (Player_GetFruitPicker(playerid) >= 1) {
                    Alert_Send(playerid, "~g~[Farm Supplier]~r~ You already have maximum pickers!", 3000);
                    ShowBuyMenu(playerid);
                    return 1;
                }
                new price = BuyPrices[0];
                if (GetPlayerMoney(playerid) < price) {
                    Alert_Send(playerid, "~g~[Farm Supplier]~r~ Not enough money!", 3000);
                    ShowBuyMenu(playerid);
                    return 1;
                }
                GivePlayerMoney(playerid, -price);
                Player_GiveFruitPicker(playerid, 1);
                new msg[128];
                format(msg, sizeof(msg), "~g~[Farm Supplier]~w~ Bought %s for $%d!", BuyNames[0], price);
                Notify_Send(playerid, msg, 3000);
            }
            else if (item == 1) {
                if (qty < 1 || qty > 25) {
                    Alert_Send(playerid, "~g~[Farm Supplier]~r~ Invalid amount! (1-25)", 3000);
                    ShowBuyMenu(playerid);
                    return 1;
                }
                new bottleCount = Player_GetBottles(playerid);
                if ((bottleCount + qty) > 25) {
                    new left = 25 - bottleCount;
                    new msg[128];
                    format(msg, sizeof(msg), "~g~[Farm Supplier]~r~ You can only buy %d more bottles!", left);
                    Alert_Send(playerid, msg, 3000);
                    ShowBuyMenu(playerid);
                    return 1;
                }
                new price = BuyPrices[1] * qty;
                if (GetPlayerMoney(playerid) < price) {
                    Alert_Send(playerid, "~g~[Farm Supplier]~r~ Not enough money!", 3000);
                    ShowBuyMenu(playerid);
                    return 1;
                }
                GivePlayerMoney(playerid, -price);
                Player_GiveBottles(playerid, qty);
                new msg[128];
                format(msg, sizeof(msg), "~g~[Farm Supplier]~w~ Bought %d bottles for $%d!", qty, price);
                Notify_Send(playerid, msg, 3000);
            }
            ShowBuyMenu(playerid);
            return 1;
        }

        case DIALOG_FARM_SELL_MENU: {
            if (!response) {
                ShowFarmMainMenu(playerid, 1);
                return 1;
            }
            if (listitem == 2) {
                ShowFarmMainMenu(playerid, 1);
                return 1;
            }

            new itemCount = 0;
            if (listitem == 0) itemCount = Player_GetOranges(playerid);
            else if (listitem == 1) itemCount = Player_GetOrangeJuice(playerid);

            if (itemCount == 0) {
                Alert_Send(playerid, "~y~[Harvest Master]~r~ You don't have this item!", 3000);
                ShowSellMenu(playerid);
                return 1;
            }

            SetPVarInt(playerid, "SellItem", listitem);
            new inputMsg[128];
            format(inputMsg, sizeof(inputMsg), "{FFAA00}%s\n{FFFFFF}You have: {00AA00}%d\nHow many to sell? (1-%d)", SellNames[listitem], itemCount, itemCount);
            ShowPlayerDialog(playerid, DIALOG_FARM_SELL_QUANTITY_INPUT, DIALOG_STYLE_INPUT,
                "{FFAA00}Sell Quantity", inputMsg, "Sell", "Cancel");
            return 1;
        }

        case DIALOG_FARM_SELL_QUANTITY_INPUT: {
            if (!response) {
                ShowSellMenu(playerid);
                return 1;
            }
            new qty = strval(inputtext);
            new item = GetPVarInt(playerid, "SellItem");
            new itemCount = 0;

            if (item == 0) itemCount = Player_GetOranges(playerid);
            else if (item == 1) itemCount = Player_GetOrangeJuice(playerid);

            if (qty < 1 || qty > itemCount) {
                Alert_Send(playerid, "~y~[Harvest Master] ~r~Invalid quantity!", 3000);
                ShowSellMenu(playerid);
                return 1;
            }

            new totalPrice = SellPrices[item] * qty;

            if (item == 0) Player_RemoveOranges(playerid, qty);
            else if (item == 1) Player_RemoveOrangeJuice(playerid, qty);

            GivePlayerMoney(playerid, totalPrice);

            new msg[128];
            format(msg, sizeof(msg), "~y~[Harvest Master]~w~ Sold %d %s for $%d!", qty, SellNames[item], totalPrice);
            Notify_Send(playerid, msg, 3000);
            ShowSellMenu(playerid);
            return 1;
        }
    }
    return 0;
}

// ==================== COMMANDS ====================

CMD:buyfarm(playerid, params[]) {
    new Float:d = GetPlayerDistanceFromPoint(playerid, SHOP_BUYER_X, SHOP_BUYER_Y, SHOP_BUYER_Z);
    if (d > 10.0) {
        Alert_Send(playerid, "~r~[ERROR]~w~ Too far!", 3000);
        return 1;
    }
    ShowFarmMainMenu(playerid, 0);
    return 1;
}

CMD:sellfarm(playerid, params[]) {
    new Float:d = GetPlayerDistanceFromPoint(playerid, SHOP_SELLER_X, SHOP_SELLER_Y, SHOP_SELLER_Z);
    if (d > 10.0) {
        Alert_Send(playerid, "~r~[ERROR]~w~ Too far!", 3000);
        return 1;
    }
    ShowFarmMainMenu(playerid, 1);
    return 1;
}
