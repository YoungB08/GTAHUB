/*
    faction_safe.inc
    ----------------
    Hệ thống Két Sắt / Kho Đồ Chung cho Faction & Gang (Faction Safes).

    Mô tả:
    - Mỗi Faction (LSPD, EMS) và Gang sẽ có một tủ đồ chung (Safe).
    - Tạo bảng `faction_inventory` lưu trữ vật phẩm theo `faction_id`.
    - Lệnh /putsafe và /takesafe để cất/lấy đồ.
    - Quyền truy cập: Chỉ những thành viên có rank nhất định (Ví dụ Rank 2+) mới được mở.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_FACTION_ITEMS       (30)    // Số vật phẩm tối đa trong kho
#define MIN_RANK_SAFE           (2)     // Rank tối thiểu để dùng tủ chung

// Vị trí tủ đồ chung (Hardcoded tạm thời, có thể dùng DB sau)
// LSPD Locker (Faction ID 1)
static const Float:LSPDSafe[3] = {1565.42, -1691.50, 5.89};
// Gang Mẫu (Faction ID 5)
static const Float:GangSafe[3] = {2496.04, -1692.93, 14.14};

// =============================================================================
// PREPARED STATEMENTS
// =============================================================================

static Statement:stmt_loadFacInv;
static Statement:stmt_insertFacInv;
static Statement:stmt_deleteFacInv;

// =============================================================================
// HOOKS
// =============================================================================

hook OnMySQLConnected()
{
    mysql_tquery(MySQL_GetHandle(),
        "CREATE TABLE IF NOT EXISTS faction_inventory (\
            id INT(11) AUTO_INCREMENT PRIMARY KEY,\
            faction_id INT(11) NOT NULL,\
            item_name VARCHAR(32) NOT NULL,\
            item_label VARCHAR(32) NOT NULL,\
            quantity INT(11) NOT NULL DEFAULT 1\
        )", "", "");

    stmt_loadFacInv = MySQL_PrepareStatement(MySQL_GetHandle(), 
        "SELECT item_name, item_label, quantity FROM faction_inventory WHERE faction_id=?");
        
    stmt_insertFacInv = MySQL_PrepareStatement(MySQL_GetHandle(),
        "INSERT INTO faction_inventory (faction_id, item_name, item_label, quantity) VALUES (?, ?, ?, ?)");
        
    stmt_deleteFacInv = MySQL_PrepareStatement(MySQL_GetHandle(),
        "DELETE FROM faction_inventory WHERE faction_id=? AND item_name=? LIMIT 1");

    return 1;
}

hook OnGameModeInit()
{
    // Tạo nhãn 3D cho các tủ đồ
    Create3DTextLabel("[LSPD LOCKER]\n{FFFFFF}/safe de truy cap", 0x0000FFFF, LSPDSafe[0], LSPDSafe[1], LSPDSafe[2], 10.0, 0, false);
    Create3DTextLabel("[GANG SAFE]\n{FFFFFF}/safe de truy cap", 0xFF0000FF, GangSafe[0], GangSafe[1], GangSafe[2], 10.0, 0, false);
    return 1;
}

// =============================================================================
// CORE FUNCTIONS
// =============================================================================

static GetPlayerSafeFaction(playerid)
{
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    
    // Kiểm tra LSPD (Team Police = 1)
    if (Player_GetClass(playerid) == TEAM_POLICE && Distance(px, py, pz, LSPDSafe[0], LSPDSafe[1], LSPDSafe[2]) <= 3.0)
    {
        return 1; // Faction ID 1
    }
    
    // Kiểm tra Gang (Ví dụ TEAM_GANG1)
    // if (Player_GetClass(playerid) == TEAM_GANG1 && Distance(px, py, pz, GangSafe[0], GangSafe[1], GangSafe[2]) <= 3.0)
    //     return 5;

    return 0; // Không đứng gần tủ đồ faction của mình
}

// =============================================================================
// COMMANDS
// =============================================================================

CMD:safe(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    new facId = GetPlayerSafeFaction(playerid);
    if (facId == 0)
        return SendErrorMsg(playerid, "Ban khong dung gan Két sắt / Locker cua to chuc ban."), 1;

    // Giả lập check rank (Chưa có hệ thống rank hoàn chỉnh cho mọi faction, tạm dùng PD Rank)
    if (Player_GetClass(playerid) == TEAM_POLICE && LSPD_GetRank(playerid) < MIN_RANK_SAFE)
        return SendErrorMsg(playerid, "Ban can Rank 2 tro len de mo Locker LSPD."), 1;

    inline OnSafeLoaded()
    {
        new content[1024];
        new count = 0;
        
        new name[32], label[32], qty;
        MySQL_BindResultString(stmt_loadFacInv, 0, name, 32);
        MySQL_BindResultString(stmt_loadFacInv, 1, label, 32);
        MySQL_BindResultInt(stmt_loadFacInv, 2, qty);
        
        format(content, sizeof(content), "=== KHO ĐỒ TỔ CHỨC ===\n/putsafe [ten do] | /takesafe [ten do]\n\n");

        while (MySQL_Statement_FetchRow(stmt_loadFacInv))
        {
            format(content, sizeof(content), "%s- %s x%d (%s)\n", content, label, qty, name);
            count++;
            
            MySQL_BindResultString(stmt_loadFacInv, 0, name, 32);
            MySQL_BindResultString(stmt_loadFacInv, 1, label, 32);
            MySQL_BindResultInt(stmt_loadFacInv, 2, qty);
        }
        
        if (count == 0) strcat(content, "{AFAFAF}Kho do dang trong.");
        
        Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Faction Safe", content, "Dong", "");
    }

    MySQL_BindInt(stmt_loadFacInv, 0, facId);
    MySQL_ExecuteParallel_Inline(stmt_loadFacInv, using inline OnSafeLoaded);

    return 1;
}

CMD:putsafe(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    new facId = GetPlayerSafeFaction(playerid);
    if (facId == 0) return SendErrorMsg(playerid, "Ban khong dung gan Két sắt / Locker cua to chuc ban."), 1;

    if (isnull(params)) return SendErrorMsg(playerid, "Su dung: /putsafe [ten slug cua vat pham]"), 1;

    if (!Inv_HasItem(playerid, params))
        return SendErrorMsg(playerid, "Ban khong co vat pham nay trong tui do."), 1;

    // Lấy label của item để lưu (Tạm fake label bằng slug)
    new label[32];
    format(label, sizeof(label), "%s", params);

    // Xóa khỏi inventory player
    Inv_RemoveItem(playerid, params, 1);

    // Lưu vào Safe
    MySQL_BindInt(stmt_insertFacInv, 0, facId);
    MySQL_BindString(stmt_insertFacInv, 1, params, 32); // item slug
    MySQL_BindString(stmt_insertFacInv, 2, label, sizeof(label));  // item label
    MySQL_BindInt(stmt_insertFacInv, 3, 1);         // qty = 1 (Tạm thời chỉ hỗ trợ cất 1 món mỗi lần)
    MySQL_ExecuteThreaded(stmt_insertFacInv);

    SendMsgF(playerid, COLOR_GREEN, "[FACTION SAFE] Ban da cat 1x %s vao kho chung.", params);
    return 1;
}

CMD:takesafe(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    new facId = GetPlayerSafeFaction(playerid);
    if (facId == 0) return SendErrorMsg(playerid, "Ban khong dung gan Két sắt / Locker cua to chuc ban."), 1;

    if (isnull(params)) return SendErrorMsg(playerid, "Su dung: /takesafe [ten slug cua vat pham]"), 1;

    // Phải kiểm tra trong DB xem có đồ không
    inline OnCheckItem()
    {
        if (MySQL_Statement_FetchRow(stmt_loadFacInv)) // Có ít nhất 1 dòng
        {
            if (Inv_AddItem(playerid, params))
            {
                // Xóa 1 dòng khỏi DB
                MySQL_BindInt(stmt_deleteFacInv, 0, facId);
                MySQL_BindString(stmt_deleteFacInv, 1, params, 32);
                MySQL_ExecuteThreaded(stmt_deleteFacInv);

                SendMsgF(playerid, COLOR_GREEN, "[FACTION SAFE] Ban da rut 1x %s tu kho chung.", params);
            }
            else
            {
                SendErrorMsg(playerid, "Tui do cua ban da day!");
            }
        }
        else
        {
            SendErrorMsg(playerid, "Khong tim thay vat pham do trong Két sắt.");
        }
    }
    
    // Tái sử dụng stmt_loadFacInv (phải đổi query một chút hoặc chạy trực tiếp inline tquery cho nhanh)
    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT id FROM faction_inventory WHERE faction_id=%d AND item_name='%e' LIMIT 1", facId, params);
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnCheckItem, query);

    return 1;
}
