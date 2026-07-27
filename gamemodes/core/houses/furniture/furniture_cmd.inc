CMD:furniture(playerid, params[]) {
    if (!Player_IsInsideHouse(playerid)) {
        return SendErrorMsg(playerid, "Ban phai o trong nha cua minh de mua noi that.");
    }

    // Initialise data
    Furniture_SetEditingState(playerid, INVALID_EDITING_TYPE);
    Player_SetFurnitureMode(playerid, false);

    inline ShowFurnitureOption(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, inputtext
        if(!response)
            return 0;

        if (!Player_IsInsideHouse(playerid)) {
            return SendErrorMsg(playerid, "Ban phai o trong nha cua minh de mua noi that.");
        }

        switch(listitem) {
            // Main Menu
            case 0: {
                Furniture_ShowDialog(playerid);
            }
            // Edit Menu
            case 1: {
                Furniture_SetEditingState(playerid, TYPE_EDITING_FURNITURE);
                Player_SetFurnitureMode(playerid, true);

                SelectObject(playerid);
                SendServerMsg(playerid, "Dung chuot de chon noi that ban muon chinh sua.");
                SendServerMsg(playerid, "Nhan ESC khi hoan tat de nhan vat co the di chuyen.");
            }
            // Delete Menu
            case 2: {
                Furniture_SetEditingState(playerid, TYPE_DELETING_FURNITURE);
                Player_SetFurnitureMode(playerid, true);

                SelectObject(playerid);
                SendServerMsg(playerid, "Dung chuot de chon noi that ban muon xoa.");
                SendServerMsg(playerid, "Nhan ESC khi hoan tat de nhan vat co the di chuyen.");
            }
        }
    }
    Dialog_ShowCallback(playerid, using inline ShowFurnitureOption, DIALOG_STYLE_LIST, "Noi That", "Mua Noi That\nSua Noi That\nXoa Noi That", "Chon", "Dong");
    return 1;
}
