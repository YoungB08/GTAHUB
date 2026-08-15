/*
    cmds_cef_test.inc
    -----------------
    Lệnh Debug & Test All CEF UI

    Cú pháp in-game:
    /cef [ui_name]  hoặc  /testcef [ui_name]

    Danh sách ui_name:
    - login        : Mở CEF Login Page
    - register     : Mở CEF Register Page
    - active       : Mở CEF Active OTP Page
    - char         : Mở CEF Character Selection
    - hud          : Mở CEF Status HUD Overlay
    - bank         : Mở CEF San Andreas Banking / ATM
    - interaction  : Mở CEF Near Range Interaction HUD (Key E)
    - inv          : Mở CEF 48-Slot Inventory
    - trunk        : Mở CEF Vehicle Trunk Storage
    - radial       : Mở CEF Dynamic Radial Menu 360
    - licenses     : Mở CEF DMV Driving School & Licenses
    - notify       : Mở CEF Toast Notification Overlay
    - phone        : Mở CEF SA Mobile Phone (iPhone SA Bank)
*/

#include <YSI_Coding\y_hooks>

CMD:cef(playerid, params[])
{
    if (isnull(params))
    {
        SendServerMsg(playerid, "===== HE THONG TEST DANH SACH CEF UI =====");
        SendClientMessage(playerid, -1, "Su dung: /cef [ten_ui]");
        SendClientMessage(playerid, -1, "Danh sach: login, register, active, char, hud, bank, interaction, inv, trunk, radial, licenses, notify, phone");
        return 1;
    }

    if (strequal(params, "login"))
    {
        CEF_ShowLoginPage(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: Login Page");
    }
    else if (strequal(params, "register"))
    {
        CEF_ShowRegisterPage(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: Register Page");
    }
    else if (strequal(params, "active"))
    {
        CEF_ShowActivePage(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: Active OTP");
    }
    else if (strequal(params, "char") || strequal(params, "character"))
    {
        CEF_ShowCharacterSelection(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: Character Selection");
    }
    else if (strequal(params, "hud"))
    {
        CEF_ShowStatusHUD(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: Status HUD");
    }
    else if (strequal(params, "bank") || strequal(params, "atm"))
    {
        CEF_ShowBanking(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: San Andreas Banking");
    }
    else if (strequal(params, "interaction") || strequal(params, "hint"))
    {
        CEF_ShowInteraction(playerid, "E", "Nha #15 - Los Santos", "Nhan E hoac Y de vao nha", "TUONG TAC GAN");
        SendServerMsg(playerid, "Da mo CEF UI: Near Range Interaction HUD (Key E)");
    }
    else if (strequal(params, "inv") || strequal(params, "inventory"))
    {
        CEF_ShowInventory(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: 48-Slot Inventory");
    }
    else if (strequal(params, "trunk"))
    {
        CEF_ShowTrunk(playerid, 0);
        SendServerMsg(playerid, "Da mo CEF UI: Vehicle Trunk");
    }
    else if (strequal(params, "radial"))
    {
        CEF_ShowRadialMenu(playerid, INVALID_PLAYER_ID);
        SendServerMsg(playerid, "Da mo CEF UI: Dynamic Radial Menu 360");
    }
    else if (strequal(params, "licenses") || strequal(params, "dmv"))
    {
        CEF_ShowLicenses(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: DMV Driving School & Licenses");
    }
    else if (strequal(params, "notify"))
    {
        CEF_ShowNotification(playerid, "success", "Thong bao test thanh cong tu CEF Notification System!", 4000);
        SendServerMsg(playerid, "Da phat CEF UI: Toast Notification");
    }
    else if (strequal(params, "phone"))
    {
        CEF_ShowPhone(playerid);
        SendServerMsg(playerid, "Da mo CEF UI: SA Mobile Phone (iPhone)");
    }
    else
    {
        SendErrorMsg(playerid, "Khong tim thay UI. Danh sach: login, register, active, char, hud, bank, interaction, inv, trunk, radial, licenses, notify, phone");
    }

    return 1;
}

CMD:testcef(playerid, params[])
{
    return cmd_cef(playerid, params);
}
