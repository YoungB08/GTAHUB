#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    // 1. Cấu hình quy tắc hiển thị chung cho toàn Server Role Component
    SetRoleGlobalConfig(25.0, true, true, 32.0, 32.0);

    // 2. Pre-register / Pre-load toàn bộ Role Icons
    AddRoleResource("admin.png",  "http://gtahub.kntech.co/icons/admin.png");
    AddRoleResource("vip.png",    "http://gtahub.kntech.co/icons/vip.png");
    AddRoleResource("mod.png",    "http://gtahub.kntech.co/icons/moderator.png");
    AddRoleResource("helper.png", "http://gtahub.kntech.co/icons/helper.png");
    AddRoleResource("dev.png",    "http://gtahub.kntech.co/icons/dev.png");

    ShowNameTags(true);
    print("[ROLE] Initialized open:mp Server Role Component.");
    return 1;
}

hook OnPlayerSpawn(playerid)
{
    // Mặc định gán Slot 0: Preset VIP khi Spawn
    SetPlayerVIPRole(playerid);
    SendClientMessage(playerid, 0x00FF00FF, "[ROLE] Ban da duoc thiet lap Slot 0 Preset VIP khi Spawn.");
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    #pragma unused reason
    ClearPlayerRoleAll(playerid);
    return 1;
}

CMD:testpreset(playerid, params[])
{
    #pragma unused params
    SetPlayerPresetRole(-1, playerid, ROLE_ADMIN, 0, 0);     // Slot 0: ADMIN
    SetPlayerPresetRole(-1, playerid, ROLE_VIP, 1, 0);       // Slot 1: VIP
    SetPlayerPresetRole(-1, playerid, ROLE_DEVELOPER, 2, 0); // Slot 2: DEV
    SendClientMessage(playerid, 0x00FFFFAFA, "[TEST] Da gan 3 Slot Preset (Slot 0: ADMIN, Slot 1: VIP, Slot 2: DEV).");
    return 1;
}

CMD:testcustom(playerid, params[])
{
    #pragma unused params
    SetPlayerCustomRole(-1, playerid, "SUPER ADMIN", 0xFFB30000, 0x88000000, true, 0, 0);
    SetPlayerCustomRole(-1, playerid, "PATRON VIP",  0xFFCC9900, 0x88000000, false, 1, 0);
    SendClientMessage(playerid, 0x00FFFFAFA, "[TEST] Da gan Custom Role Style vao Slot 0 & Slot 1.");
    return 1;
}

CMD:testimage(playerid, params[])
{
    #pragma unused params
    SetPlayerImageRole(-1, playerid, "admin.png", "LEAD ADMIN", 0xFFB30000, 0, 0);
    SendClientMessage(playerid, 0x00FFFFAFA, "[TEST] Da gan Image Role Tag (admin.png) vao Slot 0.");
    return 1;
}

CMD:testrainbow(playerid, params[])
{
    #pragma unused params
    new bool:active = IsPlayerRainbowActive(playerid, -1);
    SetPlayerRainbowRole(playerid, !active, 300, -1); // Quick Rainbow Nametag chính
    SetPlayerRainbowRole(playerid, !active, 300, 0);  // Rainbow Slot 0
    if (!active)
        SendClientMessage(playerid, 0x00FF00FF, "[TEST] Da BAT hieu ung Rainbow Cau Vong chớp màu!");
    else
        SendClientMessage(playerid, 0xFF0000FF, "[TEST] Da TAT hieu ung Rainbow Cau Vong.");
    return 1;
}

CMD:testcolor(playerid, params[])
{
    #pragma unused params
    SetPlayerNametagColor(-1, playerid, 0xFF00FFCC); // Mau Xanh Ngoc ARGB
    SendClientMessage(playerid, 0x00FFCCFF, "[TEST] Da doi mau Nametag chinh sang mau Xanh Ngoc (0xFF00FFCC)!");
    return 1;
}

CMD:testtimed(playerid, params[])
{
    #pragma unused params
    SetPlayerCustomRole(-1, playerid, "TIMED ROLE (10s)", 0xFFAA00FF, 0x88000000, true, 1, 10);
    SendClientMessage(playerid, 0xFFAA00FF, "[TEST] Da gan Role thoi han 10 giay vao Slot 1! Role se tu xoa va nhay callback.");
    return 1;
}

CMD:testundercover(playerid, params[])
{
    #pragma unused params
    new bool:visible = IsPlayerRoleVisible(playerid);
    SetPlayerRoleVisible(playerid, !visible, -1);
    if (visible)
        SendClientMessage(playerid, 0xFFFF00FF, "[TEST] Da AN Badge (Che do Undercover / Admin Off Duty).");
    else
        SendClientMessage(playerid, 0x00FF00FF, "[TEST] Da HIEN Badge (Che do Admin On Duty).");
    return 1;
}

CMD:checkrole(playerid, params[])
{
    #pragma unused params
    new bool:hasSlot0 = HasPlayerRole(playerid, 0);
    new bool:imgLoaded = IsRoleResourceLoaded("admin.png");
    new msg[128];
    format(msg, sizeof(msg), "[CHECK] Slot 0 status: %s | Icon admin.png status: %s", hasSlot0 ? "ACTIVE" : "EMPTY", imgLoaded ? "LOADED" : "NOT LOADED");
    SendClientMessage(playerid, 0x00FFFFFF, msg);
    return 1;
}

CMD:clearallroles(playerid, params[])
{
    #pragma unused params
    ClearPlayerRoleAll(playerid);
    SendClientMessage(playerid, 0xFF0000FF, "[TEST] Da xoa toan bo Role o tat ca cac Slot.");
    return 1;
}

public OnRoleResourceLoaded(playerid, const resourceKey[], bool:success)
{
    #pragma unused playerid
    new msg[128];
    format(msg, sizeof(msg), "[RESOURCE EVENT] Resource '%s' download status: %s", resourceKey, success ? "SUCCESS" : "FAILED");
    print(msg);
    return 1;
}

public OnPlayerRoleExpired(playerid, slotID)
{
    new msg[128];
    format(msg, sizeof(msg), "[ROLE EXPIRED EVENT] Player ID %d: Role at Slot %d has EXPIRED!", playerid, slotID);
    SendClientMessage(playerid, 0xFF0000FF, msg);
    print(msg);
    return 1;
}
