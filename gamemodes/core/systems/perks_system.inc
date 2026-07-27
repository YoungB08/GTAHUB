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
        "VIP cap hien tai: %d\nReward bonus VIP: %d%%\nReward bonus giay phep nghe: %s\n\nPerk dang mo khoa:\n- Mobile phone, SMS va call\n- Job XP rieng tung nghe\n- Reward scale theo level nghe\n- VIP bonus gioi han toi da 30%%\n- Poker/minigame co XP rieng\n\nPerk sap mo khoa:\n- Miner/Fisher can level 2 + giay phep nghe\n- Pilot side-job can level 3 + bang may bay",
        vipLevel,
        min(vipLevel * 10, 30),
        License_PlayerHas(playerid, LICENSE_JOB) ? ("+5%") : ("Chua co")
    );

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Perk va VIP", info, "Dong", "");
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
