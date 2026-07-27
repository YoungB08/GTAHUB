#include <YSI_Coding\y_hooks>

stock Perk_GetRewardBonusPercent(playerid)
{
    new bonus;

    if(License_PlayerHas(playerid, LICENSE_JOB)) {
        bonus += 5;
    }

    bonus += min(VIP_GetPlayerLevel(playerid) * 10, 30);
    return bonus;
}

static Perk_ShowMenu(playerid)
{
    new
        info[768],
        vipLevel = VIP_GetPlayerLevel(playerid);

    format(
        info,
        sizeof(info),
        "VIP cap hien tai: %d\nThuong them tu VIP: %d%%\nThuong them tu giay phep nghe: %s\n\nDac quyen dang mo khoa:\n- Dien thoai di dong, tin nhan va goi dien\n- XP rieng cho tung nghe\n- Thuong tang theo level nghe\n- Thuong VIP toi da 30%%\n- Poker/minigame co XP rieng\n\nDac quyen sap mo khoa:\n- Tho mo/Ngu dan can level 2 + giay phep nghe\n- Phi cong can level 3 + bang may bay",
        vipLevel,
        min(vipLevel * 10, 30),
        License_PlayerHas(playerid, LICENSE_JOB) ? ("+5%") : ("Chua co")
    );

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Dac quyen va VIP", info, "Dong", "");
    return 1;
}

CMD:perks(playerid, params[])
{
    #pragma unused params
    Perk_ShowMenu(playerid);
    return 1;
}

CMD:vipbenefits(playerid, params[])
{
    #pragma unused params
    Perk_ShowMenu(playerid);
    return 1;
}
