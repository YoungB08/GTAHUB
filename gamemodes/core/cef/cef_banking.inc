// =============================================================================
// CEF Banking - GTAHUB
// Giao diện Ngân hàng San Andreas (Withdraw, Deposit, Transfer, Bills)
// Sự kiện JS -> Pawn: banking.withdraw(amount), banking.deposit(amount), banking.transfer(targetAccount, amount, note), banking.close
// Sự kiện Pawn -> JS: banking.update(payload), banking.notify(msg, type)
// =============================================================================

#if defined _cef_banking_included
    #endinput
#endif
#define _cef_banking_included

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_BankingWithdraw(playerid, browserid, amount);
forward OnCEF_BankingDeposit(playerid, browserid, amount);
forward OnCEF_BankingTransfer(playerid, browserid, const targetAccount[], amount, const note[]);
forward OnCEF_BankingClose(playerid, browserid);

forward CEF_ShowBanking(playerid);

hook OnGameModeInit()
{
    CEF_RegisterEvent("banking.withdraw", "OnCEF_BankingWithdraw", Argument_Integer);
    CEF_RegisterEvent("banking.deposit", "OnCEF_BankingDeposit", Argument_Integer);
    CEF_RegisterEvent("banking.transfer", "OnCEF_BankingTransfer", Argument_String, Argument_Integer, Argument_String);
    CEF_RegisterEvent("banking.close", "OnCEF_BankingClose");

    print("[CEF] Banking events registered.");
    return 1;
}

stock CEF_ShowBanking(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    CEF_CreateBrowser(playerid, CEF_BROWSER_BANKING, CEF_URL_BANKING, true, true);
    return 1;
}

public OnCEF_BankingWithdraw(playerid, browserid, amount)
{
    #pragma unused browserid

    if (!IsPlayerConnected(playerid) || amount <= 0) return 0;

    new bankMoney = Player_GetBankMoney(playerid);
    if (bankMoney < amount)
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_BANKING, "banking.notify", CEF_STR("So du ngan hang khong du!"), CEF_STR("error"));
        return 1;
    }

    Player_GiveBankMoney(playerid, -amount);
    GivePlayerMoney(playerid, amount);

    CEF_EmitEvent(playerid, CEF_BROWSER_BANKING, "banking.notify", CEF_STR("Rut tien thanh cong!"), CEF_STR("success"));
    return 1;
}

public OnCEF_BankingDeposit(playerid, browserid, amount)
{
    #pragma unused browserid

    if (!IsPlayerConnected(playerid) || amount <= 0) return 0;

    new cash = GetPlayerMoney(playerid);
    if (cash < amount)
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_BANKING, "banking.notify", CEF_STR("Tien mat khong du!"), CEF_STR("error"));
        return 1;
    }

    GivePlayerMoney(playerid, -amount);
    Player_GiveBankMoney(playerid, amount);

    CEF_EmitEvent(playerid, CEF_BROWSER_BANKING, "banking.notify", CEF_STR("Nap tien thanh cong!"), CEF_STR("success"));
    return 1;
}

public OnCEF_BankingTransfer(playerid, browserid, const targetAccount[], amount, const note[])
{
    #pragma unused browserid, note

    if (!IsPlayerConnected(playerid) || amount <= 0) return 0;

    new targetId = strval(targetAccount);
    if (!IsPlayerConnected(targetId))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_BANKING, "banking.notify", CEF_STR("Nguoi nhan khong hop le!"), CEF_STR("error"));
        return 1;
    }

    new bankMoney = Player_GetBankMoney(playerid);
    if (bankMoney < amount)
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_BANKING, "banking.notify", CEF_STR("So du ngan hang khong du!"), CEF_STR("error"));
        return 1;
    }

    Player_GiveBankMoney(playerid, -amount);
    Player_GiveBankMoney(targetId, amount);

    CEF_EmitEvent(playerid, CEF_BROWSER_BANKING, "banking.notify", CEF_STR("Chuyen khoan thanh cong!"), CEF_STR("success"));
    return 1;
}

public OnCEF_BankingClose(playerid, browserid)
{
    #pragma unused browserid
    if (IsPlayerConnected(playerid))
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_BANKING);
    }
    return 1;
}
