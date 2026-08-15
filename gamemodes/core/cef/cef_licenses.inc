// =============================================================================
// CEF Licenses & DMV Driving School - GTAHUB
// Sự kiện JS -> Pawn: licenses.buy(licenseType), licenses.submitTheory(answersJson), licenses.close
// Sự kiện Pawn -> JS: licenses.show(data), licenses.startTheory(questions), licenses.theoryResult(passed, score)
// =============================================================================

#if defined _cef_licenses_included
    #endinput
#endif
#define _cef_licenses_included

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_LicensesBuy(playerid, browserid, const licenseType[]);
forward OnCEF_LicensesSubmitTheory(playerid, browserid, const answersJson[]);
forward OnCEF_LicensesClose(playerid, browserid);

forward CEF_ShowLicenses(playerid);

hook OnGameModeInit()
{
    CEF_RegisterEvent("licenses.buy", "OnCEF_LicensesBuy", Argument_String);
    CEF_RegisterEvent("licenses.submitTheory", "OnCEF_LicensesSubmitTheory", Argument_String);
    CEF_RegisterEvent("licenses.close", "OnCEF_LicensesClose");

    print("[CEF] Licenses events registered.");
    return 1;
}

stock CEF_ShowLicenses(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    CEF_CreateBrowser(playerid, CEF_BROWSER_LICENSES, CEF_URL_LICENSES, true, true);
    return 1;
}

public OnCEF_LicensesBuy(playerid, browserid, const licenseType[])
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d requested buying license: %s", playerid, licenseType);
    return 1;
}

public OnCEF_LicensesSubmitTheory(playerid, browserid, const answersJson[])
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d submitted theory test answers: %s", playerid, answersJson);

    // Trả kết quả thi đỗ mẫu
    CEF_EmitEvent(playerid, CEF_BROWSER_LICENSES, "licenses.theoryResult", CEF_BOOL(true), CEF_INT(100));
    return 1;
}

public OnCEF_LicensesClose(playerid, browserid)
{
    #pragma unused browserid
    if (IsPlayerConnected(playerid))
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_LICENSES);
    }
    return 1;
}
