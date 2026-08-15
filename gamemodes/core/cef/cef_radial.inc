// =============================================================================
// CEF Radial Menu 360° - GTAHUB
// Hỗ trợ hàm Pawn dựng phân khu & danh sách tính năng động (Dynamic Builder)
//
// Cách dùng trong Pawn:
//   Radial_Begin(playerid, targetid);
//   Radial_AddSector(playerid, "citizen", "CONG DAN", "../shared/svg/id-card.svg");
//   Radial_AddItem(playerid, "Dua tien", "radial.finance.giveMoney");
//   Radial_AddItem(playerid, "Xem ID", "radial.documents.showID");
//   Radial_EndSector(playerid);
//   Radial_Send(playerid);
//
// Sự kiện JS -> Pawn: radial.select(action, targetId), radial.sectorClick(sectorKey, targetId), ui.radial.close
// Sự kiện Pawn -> JS: radial.open(targetId), radial.setSectors(sectorsJson)
// =============================================================================

#if defined _cef_radial_included
    #endinput
#endif
#define _cef_radial_included

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_RadialSelect(playerid, browserid, const action[], targetId);
forward OnCEF_RadialSectorClick(playerid, browserid, const sectorKey[], targetId);
forward OnCEF_RadialClose(playerid, browserid);

forward OnPlayerRadialAction(playerid, targetid, const action[]);
forward OnPlayerRadialSectorClick(playerid, targetid, const sectorKey[]);

// --- State tracking per player for dynamic builder ---
static gRadialTargetId[MAX_PLAYERS];
static gRadialJsonBuffer[MAX_PLAYERS][2048];
static bool:gRadialHasSectors[MAX_PLAYERS];

hook OnPlayerConnect(playerid)
{
    gRadialTargetId[playerid] = INVALID_PLAYER_ID;
    gRadialJsonBuffer[playerid][0] = '\0';
    gRadialHasSectors[playerid] = false;
    return 1;
}

hook OnGameModeInit()
{
    // Đăng ký event từ JS emit
    CEF_RegisterEvent("radial.select", "OnCEF_RadialSelect", Argument_String, Argument_Integer);
    CEF_RegisterEvent("radial.sectorClick", "OnCEF_RadialSectorClick", Argument_String, Argument_Integer);
    CEF_RegisterEvent("ui.radial.close", "OnCEF_RadialClose");

    print("[CEF] Dynamic Radial Menu events registered.");
    return 1;
}

// =============================================================================
// PAWN DYNAMIC RADIAL MENU BUILDER API
// =============================================================================

// 1. Khởi tạo phiên dựng Radial Menu động cho playerid
stock Radial_Begin(playerid, targetid = INVALID_PLAYER_ID)
{
    if (!IsPlayerConnected(playerid)) return 0;
    gRadialTargetId[playerid] = targetid;
    gRadialHasSectors[playerid] = false;
    format(gRadialJsonBuffer[playerid], sizeof(gRadialJsonBuffer[]), "{");
    return 1;
}

// 2. Thêm một phân khu chính (Sector) kèm Icon và Tiêu đề
stock Radial_AddSector(playerid, const sectorKey[], const title[], const icon[] = "")
{
    if (!IsPlayerConnected(playerid)) return 0;
    
    new str[256];
    if (gRadialHasSectors[playerid])
    {
        strcat(gRadialJsonBuffer[playerid], ",");
    }

    if (isnull(icon))
    {
        format(str, sizeof(str), "\"%s\":{\"title\":\"%s\",\"items\":[", sectorKey, title);
    }
    else
    {
        format(str, sizeof(str), "\"%s\":{\"title\":\"%s\",\"icon\":\"%s\",\"items\":[", sectorKey, title, icon);
    }

    strcat(gRadialJsonBuffer[playerid], str);
    gRadialHasSectors[playerid] = true;
    return 1;
}

// 3. Thêm một tính năng / nút bấm (Sub-item) vào phân khu hiện tại
stock Radial_AddItem(playerid, const label[], const actionKey[])
{
    if (!IsPlayerConnected(playerid)) return 0;

    new len = strlen(gRadialJsonBuffer[playerid]);
    new str[256];

    if (len > 0 && gRadialJsonBuffer[playerid][len - 1] == '[')
    {
        format(str, sizeof(str), "{\"label\":\"%s\",\"action\":\"%s\"}", label, actionKey);
    }
    else
    {
        format(str, sizeof(str), ",{\"label\":\"%s\",\"action\":\"%s\"}", label, actionKey);
    }

    strcat(gRadialJsonBuffer[playerid], str);
    return 1;
}

// 4. Đóng phân khu
stock Radial_EndSector(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;
    strcat(gRadialJsonBuffer[playerid], "]}");
    return 1;
}

// 5. Gửi Radial Menu động sang CEF JS
stock Radial_Send(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    strcat(gRadialJsonBuffer[playerid], "}");

    CEF_CreateBrowser(playerid, CEF_BROWSER_RADIAL, CEF_URL_RADIAL, true, false);
    CEF_EmitEvent(playerid, CEF_BROWSER_RADIAL, "radial.open", CEF_INT(gRadialTargetId[playerid]));
    CEF_EmitEvent(playerid, CEF_BROWSER_RADIAL, "radial.setSectors", CEF_STR(gRadialJsonBuffer[playerid]));
    return 1;
}

// --- Mở Menu mặc định hoặc truyền JSON trực tiếp ---
stock CEF_ShowRadialMenu(playerid, targetid, const sectorsJson[] = "")
{
    if (!IsPlayerConnected(playerid)) return 0;

    if (!isnull(sectorsJson))
    {
        CEF_CreateBrowser(playerid, CEF_BROWSER_RADIAL, CEF_URL_RADIAL, true, false);
        CEF_EmitEvent(playerid, CEF_BROWSER_RADIAL, "radial.open", CEF_INT(targetid));
        CEF_EmitEvent(playerid, CEF_BROWSER_RADIAL, "radial.setSectors", CEF_STR(sectorsJson));
    }
    else
    {
        // Mẫu dựng 4 phân khu chuẩn khi mở không truyền tham số
        Radial_Begin(playerid, targetid);

        Radial_AddSector(playerid, "citizen", "CONG DAN", "../shared/svg/id-card.svg");
        Radial_AddItem(playerid, "Dua tien", "radial.finance.giveMoney");
        Radial_AddItem(playerid, "Xem ID", "radial.documents.showID");
        Radial_AddItem(playerid, "Xuat trinh bang lai", "radial.documents.showDriverLicense");
        Radial_EndSector(playerid);

        Radial_AddSector(playerid, "blips", "DIEM DANH DAU", "../shared/svg/inspect.svg");
        Radial_AddItem(playerid, "Don canh sat", "radial.blips.policeStation");
        Radial_AddItem(playerid, "Benh vien", "radial.blips.hospital");
        Radial_AddItem(playerid, "Ngan hang", "radial.blips.bank");
        Radial_EndSector(playerid);

        Radial_AddSector(playerid, "general", "TONG QUAT", "../shared/svg/vehicle.svg");
        Radial_AddItem(playerid, "Trao vat pham", "radial.inventory.giveItem");
        Radial_AddItem(playerid, "Mo cop xe", "radial.vehicle.openTrunk");
        Radial_AddItem(playerid, "Moi len xe", "radial.vehicle.inviteVehicle");
        Radial_EndSector(playerid);

        Radial_AddSector(playerid, "police", "CANH SAT", "../shared/svg/police.svg");
        Radial_AddItem(playerid, "Cong tay", "radial.action.cuff");
        Radial_AddItem(playerid, "Kham nguoi", "radial.inspect.inspectPerson");
        Radial_EndSector(playerid);

        Radial_Send(playerid);
    }
    return 1;
}

// =============================================================================
// CALLBACKS XỬ LÝ SỰ KIỆN TỪ CLIENT
// =============================================================================

// Callback khi Player click 1 tính năng / item
public OnCEF_RadialSelect(playerid, browserid, const action[], targetId)
{
    #pragma unused browserid

    if (!IsPlayerConnected(playerid)) return 0;

    printf("[CEF Radial] Player %d executed Action: %s on Target %d", playerid, action, targetId);

    // Phát callback local để các hệ thống khác hook xử lý
    CallLocalFunction("OnPlayerRadialAction", "iis", playerid, targetId, action);

    CEF_DestroyBrowser(playerid, CEF_BROWSER_RADIAL);
    return 1;
}

// Callback khi Player click vào 1 phân khu chính
public OnCEF_RadialSectorClick(playerid, browserid, const sectorKey[], targetId)
{
    #pragma unused browserid

    if (!IsPlayerConnected(playerid)) return 0;

    printf("[CEF Radial] Player %d clicked Sector: %s (Target: %d)", playerid, sectorKey, targetId);

    // Phát callback local để gamemode xử lý thêm logic khi click phân khu
    CallLocalFunction("OnPlayerRadialSectorClick", "iis", playerid, targetId, sectorKey);
    return 1;
}

public OnCEF_RadialClose(playerid, browserid)
{
    #pragma unused browserid

    if (IsPlayerConnected(playerid))
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_RADIAL);
    }
    return 1;
}
