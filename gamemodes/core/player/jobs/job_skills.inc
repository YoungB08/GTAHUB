/*
    job_skills.inc
    --------------
    Hệ thống Level & XP Nghề Nghiệp (Job Skill Levels & XP).

    Mô tả:
    - Mỗi nghề có hệ thống XP và Level riêng (Level 1 -> 5).
    - Hoàn thành 1 đơn hàng → cộng XP.
    - Level cao → Tiền thưởng mỗi đơn được nhân thêm (bonus %).
    - Ngưỡng XP mỗi Level: Level 1=100 XP, 2=200 XP, 3=300 XP, 4=400 XP, 5=MAX.
    - /myjobrank: Xem XP và Level nghề hiện tại.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

// Job IDs (khớp với Job system hiện tại)
#define JOB_TRUCKER         (1)
#define JOB_PIZZA           (2)
#define JOB_GARBAGE         (3)
#define JOB_ROADCLEANER     (4)
#define JOB_MECHANIC        (5)

#define JOB_MAX_LEVEL       (5)

// XP cộng mỗi lần hoàn thành đơn
#define XP_PER_DELIVERY     (25)

// Bonus % tiền thưởng theo Level (0-indexed: [0] = Level 1)
static const Float:LevelBonus[JOB_MAX_LEVEL] = {0.0, 0.10, 0.20, 0.35, 0.50};

// XP cần thiết để lên mỗi Level
static const LevelXPThreshold[JOB_MAX_LEVEL] = {100, 200, 300, 400, 999999};

// =============================================================================
// DATA
// =============================================================================

static PlayerJobXP[MAX_PLAYERS][JOB_MAX_LEVEL + 1];    // XP mỗi nghề [playerid][jobid]
static PlayerJobLevel[MAX_PLAYERS][JOB_MAX_LEVEL + 1]; // Level mỗi nghề

static Statement:stmt_loadJobSkills;
static Statement:stmt_saveJobSkills;

// =============================================================================
// HOOKS – LOAD/SAVE
// =============================================================================

hook OnMySQLConnected()
{
    mysql_tquery(MySQL_GetHandle(),
        "CREATE TABLE IF NOT EXISTS job_skills (\
            id INT(11) AUTO_INCREMENT PRIMARY KEY,\
            char_id INT(11) NOT NULL DEFAULT 0,\
            job_id TINYINT(4) NOT NULL DEFAULT 0,\
            xp INT(11) NOT NULL DEFAULT 0,\
            level TINYINT(4) NOT NULL DEFAULT 1,\
            UNIQUE KEY char_job (char_id, job_id)\
        )", "", "");

    stmt_loadJobSkills = MySQL_PrepareStatement(MySQL_GetHandle(),
        "SELECT job_id, xp, level FROM job_skills WHERE char_id=?");

    stmt_saveJobSkills = MySQL_PrepareStatement(MySQL_GetHandle(),
        "INSERT INTO job_skills (char_id, job_id, xp, level) VALUES (?, ?, ?, ?) \
         ON DUPLICATE KEY UPDATE xp=VALUES(xp), level=VALUES(level)");
    return 1;
}

hook OnPlayerConnect(playerid)
{
    for (new i = 0; i <= JOB_MAX_LEVEL; i++)
    {
        PlayerJobXP[playerid][i]    = 0;
        PlayerJobLevel[playerid][i] = 1;
    }
    return 1;
}

// Gọi khi người chơi đã load xong nhân vật
hook OnCharacterLoaded(playerid, charid)
{
    inline OnLoadJobSkills()
    {
        new jobId, xp, lvl;
        MySQL_BindResultInt(stmt_loadJobSkills, 0, jobId);
        MySQL_BindResultInt(stmt_loadJobSkills, 1, xp);
        MySQL_BindResultInt(stmt_loadJobSkills, 2, lvl);
        while (MySQL_Statement_FetchRow(stmt_loadJobSkills))
        {
            if (jobId >= 1 && jobId <= JOB_MAX_LEVEL)
            {
                PlayerJobXP[playerid][jobId]    = xp;
                PlayerJobLevel[playerid][jobId] = lvl;
            }
            MySQL_BindResultInt(stmt_loadJobSkills, 0, jobId);
            MySQL_BindResultInt(stmt_loadJobSkills, 1, xp);
            MySQL_BindResultInt(stmt_loadJobSkills, 2, lvl);
        }
    }
    MySQL_BindInt(stmt_loadJobSkills, 0, charid);
    MySQL_ExecuteParallel_Inline(stmt_loadJobSkills, using inline OnLoadJobSkills);
    return 1;
}

static JobSkills_Save(playerid)
{
    new charid = Account_GetCharID(playerid);
    for (new jobId = 1; jobId <= JOB_MAX_LEVEL; jobId++)
    {
        MySQL_BindInt(stmt_saveJobSkills, 0, charid);
        MySQL_BindInt(stmt_saveJobSkills, 1, jobId);
        MySQL_BindInt(stmt_saveJobSkills, 2, PlayerJobXP[playerid][jobId]);
        MySQL_BindInt(stmt_saveJobSkills, 3, PlayerJobLevel[playerid][jobId]);
        MySQL_ExecuteThreaded(stmt_saveJobSkills);
    }
}

hook OnPlayerDisconnect(playerid, reason)
{
    if (Player_IsLoggedIn(playerid))
        JobSkills_Save(playerid);
    return 1;
}

// =============================================================================
// API – ĐƯỢC GỌI TỪ CÁC JOB KHI HOÀN THÀNH ĐƠN HÀNG
// =============================================================================

// Hàm chính: Cộng XP và kiểm tra Level Up
// Trả về: Bonus tiền (số tiền THÊM vào so với reward gốc)
stock JobSkills_AddXP(playerid, jobId)
{
    if (jobId < 1 || jobId > JOB_MAX_LEVEL) return 0;

    PlayerJobXP[playerid][jobId] += XP_PER_DELIVERY;

    new currentLevel = PlayerJobLevel[playerid][jobId];
    
    // Kiểm tra Level Up
    if (currentLevel < JOB_MAX_LEVEL && 
        PlayerJobXP[playerid][jobId] >= LevelXPThreshold[currentLevel - 1])
    {
        PlayerJobLevel[playerid][jobId]++;
        new newLevel = PlayerJobLevel[playerid][jobId];
        
        SendMsgF(playerid, 0xFFD700FF, "====================================");
        SendMsgF(playerid, 0xFFD700FF, "   LEVEL UP NGHE NGHIEP! Level %d!", newLevel);
        SendMsgF(playerid, 0xFFFFFFFF, "   Bonus: +%.0f%% tien thuong moi don.", LevelBonus[newLevel - 1] * 100.0);
        SendMsgF(playerid, 0xFFD700FF, "====================================");
        
        GameTextForPlayer(playerid, "~y~LEVEL UP!", 3000, 6);
    }
    
    return 1;
}

// Hàm lấy số tiền bonus dựa trên Level
stock JobSkills_GetBonusMoney(playerid, jobId, baseReward)
{
    if (jobId < 1 || jobId > JOB_MAX_LEVEL) return 0;
    new level = PlayerJobLevel[playerid][jobId];
    return floatround(float(baseReward) * LevelBonus[level - 1]);
}

stock JobSkills_GetLevel(playerid, jobId) return PlayerJobLevel[playerid][jobId];
stock JobSkills_GetXP(playerid, jobId)    return PlayerJobXP[playerid][jobId];

// =============================================================================
// COMMAND
// =============================================================================

CMD:myjobrank(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    new content[512];
    strcat(content, "{FFFF00}=== BANG THANH TICH NGHE NGHIEP ===\n\n");

    new const JobNames[JOB_MAX_LEVEL + 1][] = {
        "", "Xe Tai (Trucker)", "Giao Pizza", "Xe Ra? c", "Quet Duong", "Tho May"
    };

    for (new i = 1; i <= JOB_MAX_LEVEL; i++)
    {
        new level = PlayerJobLevel[playerid][i];
        new xp    = PlayerJobXP[playerid][i];
        new needed = (level < JOB_MAX_LEVEL) ? LevelXPThreshold[level - 1] : 9999;

        format(content, sizeof(content), "%s{FFFFFF}%s: Level %d | XP: %d/%d | Bonus: +%.0f%%\n",
            content, JobNames[i], level, xp, needed, LevelBonus[level - 1] * 100.0);
    }

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Thanh Tich Nghe Nghiep", content, "Dong", "");
    return 1;
}
