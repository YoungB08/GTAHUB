/*
    player_interaction_menu.inc
    ----------------------------
    Menu Tương Tác Trực Quan Giữa Người Chơi (Player Interaction Menu).

    Mô tả:
    - Lệnh /interact [ID]: Mở Dialog list các hành động tương tác với người chơi gần đó.
    - Danh sách tùy biến theo Class của người dùng:
        - Tất cả: Xem thông tin, Trả tiền, Đưa vật phẩm.
        - LSPD: Thêm Còng tay (Cuff), Dẫn giải (Escort), Khám người (Inspect).
        - EMS: Thêm Băng bó (Bandage), Hồi sức (CPR).
*/

#include <YSI_Coding\y_hooks>

// Khoảng cách tối đa để tương tác
#define INTERACT_MAX_DIST   (5.0)

// =============================================================================
// COMMAND
// =============================================================================

CMD:interact(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /interact [ID]"), 1;

    if (!IsPlayerConnected(targetid) || targetid == playerid)
        return SendErrorMsg(playerid, "Nguoi choi khong hop le."), 1;

    if (!Player_IsNearPlayer(playerid, targetid, INTERACT_MAX_DIST))
        return SendErrorMsgF(playerid, "Ban qua xa nguoi choi nay! (Can o trong %.0fm.)", INTERACT_MAX_DIST), 1;

    // Xây danh sách tương tác
    new content[512];
    new itemCount = 0;
    
    new name[MAX_PLAYER_NAME];
    GetPlayerName(targetid, name, sizeof(name));

    // --- Chung cho mọi người ---
    format(content, sizeof(content), 
        "Xem thong tin\nTra tien mat\nDua vat pham");
    itemCount = 3;

    // --- LSPD On Duty ---
    new bool:isPolice = (Player_GetClass(playerid) == TEAM_POLICE && LspdOnDuty[playerid]);
    if (isPolice)
    {
        strcat(content, "\n[LSPD] Khoa tay (Cuff)\n[LSPD] Dan giai (Escort)\n[LSPD] Kham nguoi (Inspect)");
        itemCount += 3;
    }

    // --- EMS On Duty ---
    new bool:isEMS = (Player_GetClass(playerid) == TEAM_MEDIC && EMS_IsOnDuty(playerid));
    if (isEMS)
    {
        strcat(content, "\n[EMS] Bang bo (Bandage)\n[EMS] Hoi suc tim (CPR)");
        itemCount += 2;
    }

    SetPVarInt(playerid, "InteractTarget", targetid);
    SetPVarInt(playerid, "InteractIsPolice", isPolice ? 1 : 0);
    SetPVarInt(playerid, "InteractIsEMS",    isEMS ? 1 : 0);

    new title[64];
    format(title, sizeof(title), "Tuong Tac: %s (ID %d)", name, targetid);

    inline OnInteract(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response) return;

        new target = GetPVarInt(playerid, "InteractTarget");
        new police = GetPVarInt(playerid, "InteractIsPolice");
        new ems    = GetPVarInt(playerid, "InteractIsEMS");

        if (!IsPlayerConnected(target))
        {
            SendErrorMsg(playerid, "Nguoi choi da thoat game.");
            return;
        }

        if (!Player_IsNearPlayer(playerid, target, INTERACT_MAX_DIST))
        {
            SendErrorMsg(playerid, "Nguoi choi da di qua xa!");
            return;
        }

        // ---- Mục chung ----
        if (listitem == 0) // Xem thông tin
        {
            new info[512];
            new tname[MAX_PLAYER_NAME];
            GetPlayerName(target, tname, sizeof(tname));
            format(info, sizeof(info),
                "Ten IC: %s\nClass: %d\nHP: %.1f | Armor: %.1f\nMoney: $%d\n\nGiay phep:\n- Bang lai xe: %s\n- GPDKD: %s",
                tname,
                Player_GetClass(target),
                _:GetPlayerHealthFloat(target),
                _:GetPlayerArmourFloat(target),
                GetPlayerMoney(target),
                Player_HasLicense(target, LICENSE_CAR) ? "Co" : "Khong",
                Player_HasBizLicense(target)           ? "Co" : "Khong"
            );
            Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Thong Tin Nguoi Choi", info, "Dong", "");
        }
        else if (listitem == 1) // Trả tiền
        {
            inline OnPayInput(res, li, string:txt[])
            {
                #pragma unused li
                if (!res) return;
                new amount = strval(txt);
                if (amount <= 0 || amount > GetPlayerMoney(playerid))
                {
                    SendErrorMsg(playerid, "So tien khong hop le.");
                    return;
                }
                GivePlayerMoney(playerid, -amount);
                GivePlayerMoney(target, amount);
                new tname2[MAX_PLAYER_NAME];
                GetPlayerName(target, tname2, sizeof(tname2));
                SendMsgF(playerid, COLOR_GREEN, "[Tra Tien] Da tra $%d cho %s.", amount, tname2);
                SendMsgF(target, COLOR_GREEN, "[Nhan Tien] %s da tra cho ban $%d.", ReturnPlayerName(playerid), amount);
            }
            Dialog_ShowCallback(playerid, using inline OnPayInput, DIALOG_STYLE_INPUT, "Tra Tien Mat", "Nhap so tien muon tra:", "Xac Nhan", "Huy");
        }
        else if (listitem == 2) // Đưa vật phẩm
        {
            inline OnGiveInput(res, li, string:txt[])
            {
                #pragma unused li
                if (!res) return;
                if (!Inv_HasItem(playerid, txt))
                {
                    SendErrorMsg(playerid, "Ban khong co vat pham nay trong tui.");
                    return;
                }
                if (!Inv_AddItem(target, txt))
                {
                    SendErrorMsg(playerid, "Tui do cua ho da day!");
                    return;
                }
                Inv_RemoveItem(playerid, txt, 1);
                SendMsgF(playerid, COLOR_GREEN, "[Dua Do] Da dua 1x '%s' cho %s.", txt, ReturnPlayerName(target));
                SendMsgF(target, COLOR_GREEN, "[Nhan Do] %s da dua cho ban 1x '%s'.", ReturnPlayerName(playerid), txt);
            }
            Dialog_ShowCallback(playerid, using inline OnGiveInput, DIALOG_STYLE_INPUT, "Dua Vat Pham", "Nhap ten slug cua vat pham:", "Xac Nhan", "Huy");
        }
        else
        {
            // Tính offset cho LSPD/EMS
            new offset = 3; // sau 3 mục chung
            if (police)
            {
                if (listitem == offset + 0) // Còng tay
                    cmd_cuff(playerid, ReturnPlayerID(target));
                else if (listitem == offset + 1) // Dẫn giải
                    cmd_escort(playerid, ReturnPlayerID(target));
                else if (listitem == offset + 2) // Khám người
                    cmd_inspect(playerid, ReturnPlayerID(target));
                offset += 3;
            }
            if (ems)
            {
                if (listitem == offset + 0) // Băng bó
                    cmd_bandage(playerid, ReturnPlayerID(target));
                else if (listitem == offset + 1) // CPR
                    cmd_cpr(playerid, ReturnPlayerID(target));
            }
        }
    }

    Dialog_ShowCallback(playerid, using inline OnInteract, DIALOG_STYLE_LIST, title, content, "Thuc Hien", "Huy");
    return 1;
}

// Helper: trả về playerid dạng string để truyền vào cmd_*
static ReturnPlayerID(playerid)
{
    static str[8];
    format(str, sizeof(str), "%d", playerid);
    return str;
}
