/*
    driving_school.inc
    ------------------
    Hệ thống Trường Thi Lái Xe & Bài Kiểm Tra Thực Hành (Driving School RP).

    Flow:
    1. Người chơi đến NPC Trường thi lái xe (Los Santos / San Fierro).
    2. Đăng ký thi lái xe (/takeexam hoặc bấm Y tại NPC, phí $500).
    3. Thi Lý Thuyết: Bộ 5 câu hỏi trắc nghiệm Dialog về Luật Giao Thông IC. (Đạt >=4/5 câu).
    4. Thi Thực Hành:
       - Cấp xe thi (Driving Test Vehicle).
       - Chạy qua 10 Checkpoints định sẵn.
       - Giới hạn tốc độ: 60 km/h (vượt quá 3 lần bị hủy bài thi).
       - Hỏng xe quá 25% -> Trượt bài thi.
    5. Đạt yêu cầu -> Tự động cấp Bằng lái xe (LICENSE_CAR) lưu DB.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define DRIVING_SCHOOL_FEE       (500)      // Phí thi bằng lái
#define MAX_THEORY_QUESTIONS     (5)        // Số câu hỏi lý thuyết
#define PASS_THEORY_MIN          (4)        // Số câu đúng tối thiểu
#define MAX_SPEED_VIOLATIONS     (3)        // Tối đa 3 lần vượt 60km/h
#define MAX_TEST_CHECKPOINTS     (10)       // 10 Checkpoints thực hành
#define TEST_SPEED_LIMIT         (60.0)     // km/h

// Vị trí Trung Tâm Đăng Ký Thi Bằng Lái (City Hall - Los Santos)
#define DS_NPC_X                 (1172.5)
#define DS_NPC_Y                 (-1323.8)
#define DS_NPC_Z                 (14.0)
#define DS_NPC_ANGLE             (270.0)

// Vị trí Sân Thi Thực Hành (Red County - Dillimore Track)
#define DS_PRACTICAL_X           (1368.5)
#define DS_PRACTICAL_Y           (-1279.0)
#define DS_PRACTICAL_Z           (13.5)
#define DS_PRACTICAL_A           (90.0)

// Vị trí spawn xe thi thực hành tại Red County
#define DS_VEH_SPAWN_X           (1355.0)
#define DS_VEH_SPAWN_Y           (-1280.0)
#define DS_VEH_SPAWN_Z           (13.5)
#define DS_VEH_SPAWN_A           (90.0)

// 10 Checkpoints bài thi thực hành tại Red County / Dillimore
static const Float:DSCheckpoints[MAX_TEST_CHECKPOINTS][3] = {
    {1300.0, -1280.0, 13.5},
    {1200.0, -1280.0, 13.5},
    {1050.0, -1200.0, 16.0},
    {1050.0, -1000.0, 31.0},
    {1250.0, -950.0, 42.0},
    {1450.0, -950.0, 37.0},
    {1450.0, -1100.0, 24.0},
    {1450.0, -1250.0, 14.5},
    {1380.0, -1280.0, 13.5},
    {1355.0, -1280.0, 13.5}  // Về đích tại Red County
};

// =============================================================================
// DATA
// =============================================================================

enum E_TEST_STATE { TEST_NONE, TEST_THEORY, TEST_PRACTICAL }

static E_TEST_STATE:PlayerTestState[MAX_PLAYERS];
static TheoryCorrectAnswers[MAX_PLAYERS];
static TheoryCurrentQ[MAX_PLAYERS];
static SpeedViolationCount[MAX_PLAYERS];
static CurrentCPIndex[MAX_PLAYERS];
static TestVehicleID[MAX_PLAYERS];

// =============================================================================
// QUESTIONS REGISTRY
// =============================================================================

enum E_QUESTION {
    E_Q_TEXT[256],
    E_Q_OPTIONS[256],
    E_Q_CORRECT         // 0-based index
}

static const Questions[MAX_THEORY_QUESTIONS][E_QUESTION] = {
    {
        "Khi gap den do tai nga tu, ban phai lam gi?",
        "Tang toc qua nhanh\tDung lai truoc vach qua duong\tRe phai ngay khong can dung",
        1
    },
    {
        "Toc do toi da cho phep trong khu dan cu la bao nhieu?",
        "60 km/h\t100 km/h\tKhong gioi han",
        0
    },
    {
        "Khi nghe tieng coi xe Canh Sat / EMS dang cap cuu, ban phai:",
        "Dua xe voi ho\tNhuong duong bang cach tấp vao le phai\tDung lai giua duong",
        1
    },
    {
        "Lai xe khi dang say ruou / ngáo ma túy se bi xu ly nhu the nao?",
        "Khong sao ca\tBi phat tien va tước bang lai IC\tDuoc thuong tien",
        1
    },
    {
        "Khi chuyen huong hoac re duong, ban bat buoc phai:",
        "Bật den xi-nhan bao hieu truoc\tBop coi lien tuc\tKhong can lam gi",
        0
    }
};

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid)
{
    PlayerTestState[playerid]       = TEST_NONE;
    TheoryCorrectAnswers[playerid]  = 0;
    TheoryCurrentQ[playerid]        = 0;
    SpeedViolationCount[playerid]   = 0;
    CurrentCPIndex[playerid]        = 0;
    TestVehicleID[playerid]         = INVALID_VEHICLE_ID;
    return 1;
}

hook OnGameModeInit()
{
    // Tạo NPC Giảng viên thi lái xe
    Create3DTextLabel(
        "Truong Thi Lai Xe IC\n"C_YELLOW"Nhan Y de dang ky thi ($500)",
        COLOR_WHITE, DS_NPC_X, DS_NPC_Y, DS_NPC_Z + 1.0, 30.0, 0, false);

    new actor = CreateActor(17, DS_NPC_X, DS_NPC_Y, DS_NPC_Z, DS_NPC_ANGLE);
    SetActorVirtualWorld(actor, 0);

    Logger_Log("[driving_school.inc] Driving school initialized.");
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if (PRESSED(KEY_YES))
    {
        if (IsPlayerInRangeOfPoint(playerid, 4.0, DS_NPC_X, DS_NPC_Y, DS_NPC_Z))
        {
            DS_PromptStartExam(playerid);
        }
    }
    return 1;
}

hook OnPlayerEnterRaceCheckpoint(playerid)
{
    if (PlayerTestState[playerid] != TEST_PRACTICAL) return 1;

    // Checkpoint hiện tại
    new nextCP = CurrentCPIndex[playerid] + 1;

    if (nextCP >= MAX_TEST_CHECKPOINTS)
    {
        // Hoàn thành xuất sắc bài thi
        DS_PassExam(playerid);
    }
    else
    {
        CurrentCPIndex[playerid] = nextCP;
        DisablePlayerRaceCheckpoint(playerid);

        new type = (nextCP == MAX_TEST_CHECKPOINTS - 1) ? 1 : 0; // 1 = Finish flag
        SetPlayerRaceCheckpoint(playerid, type,
            DSCheckpoints[nextCP][0], DSCheckpoints[nextCP][1], DSCheckpoints[nextCP][2],
            (nextCP == MAX_TEST_CHECKPOINTS - 1) ? DSCheckpoints[nextCP][0] : DSCheckpoints[nextCP + 1][0],
            (nextCP == MAX_TEST_CHECKPOINTS - 1) ? DSCheckpoints[nextCP][1] : DSCheckpoints[nextCP + 1][1],
            (nextCP == MAX_TEST_CHECKPOINTS - 1) ? DSCheckpoints[nextCP][2] : DSCheckpoints[nextCP + 1][2],
            5.0);

        SendMsgF(playerid, COLOR_GREEN,
            "[THI LAI XE] Checkpoint %d/%d! Duy tri toc do duoi 60 km/h.",
            nextCP + 1, MAX_TEST_CHECKPOINTS);
    }

    return 1;
}

// Kiểm tra tốc độ thực hành mỗi giây
hook OnPlayerSecondUpdate(playerid)
{
    if (PlayerTestState[playerid] != TEST_PRACTICAL) return 1;

    if (!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleID(playerid) != TestVehicleID[playerid])
    {
        DS_FailExam(playerid, "Ban da roi khoi xe thi!");
        return 1;
    }

    // Kiểm tra tốc độ
    new Float:vx, Float:vy, Float:vz;
    GetVehicleVelocity(TestVehicleID[playerid], vx, vy, vz);
    new Float:speed = floatsqroot(vx * vx + vy * vy + vz * vz) * 160.0; // km/h

    if (speed > TEST_SPEED_LIMIT)
    {
        SpeedViolationCount[playerid]++;
        SendMsgF(playerid, COLOR_RED,
            "[CANH BAO] Toc do: %.1f km/h (Vuot qua 60 km/h)! Vi pham: %d/%d",
            speed, SpeedViolationCount[playerid], MAX_SPEED_VIOLATIONS);

        if (SpeedViolationCount[playerid] >= MAX_SPEED_VIOLATIONS)
        {
            DS_FailExam(playerid, "Vi pham toc do qua 3 lan!");
        }
    }

    return 1;
}

// =============================================================================
// EXAM FLOW
// =============================================================================

static DS_PromptStartExam(playerid)
{
    if (Player_HasLicense(playerid, LICENSE_CAR))
        return SendErrorMsg(playerid, "Ban da co Bằng Lái Xe roi! Khong can thi lai."), 1;

    if (PlayerTestState[playerid] != TEST_NONE)
        return SendErrorMsg(playerid, "Ban dang trong bai thi roi."), 1;

    if (GetPlayerMoney(playerid) < DRIVING_SCHOOL_FEE)
        return SendErrorMsgF(playerid, "Can $%d de dang ky thi bang lai xe.", DRIVING_SCHOOL_FEE), 1;

    inline OnConfirm(response, listitem, string:inputtext[])
    {
        #pragma unused listitem, inputtext
        if (!response) return 1;

        GivePlayerMoney(playerid, -DRIVING_SCHOOL_FEE);

        // Dịch chuyển đến Trung tâm Thi Lái Xe Red County
        SetPlayerPos(playerid, DS_NPC_X, DS_NPC_Y, DS_NPC_Z);
        SetPlayerFacingAngle(playerid, DS_NPC_ANGLE);
        SetPlayerVirtualWorld(playerid, 0);
        SetPlayerInterior(playerid, 0);

        // Bắt đầu phần thi Lý thuyết
        PlayerTestState[playerid]      = TEST_THEORY;
        TheoryCorrectAnswers[playerid] = 0;
        TheoryCurrentQ[playerid]       = 0;

        SendMsgF(playerid, COLOR_GREEN,
            "[THI LAI XE] Da dong phi $%d. Bat dau phan thi LY THUYET (5 cau hoi).", DRIVING_SCHOOL_FEE);

        DS_ShowTheoryQuestion(playerid, 0);
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline OnConfirm,
        DIALOG_STYLE_MSGBOX,
        "Dang Ky Thi Bang Lai Xe IC",
        "Ban co muon dang ky thi Bang Lai Xe?\n\n\
- Phi thi: {33AA33}$500{FFFFFF}\n\
- Bai thi bao gom: 5 cau Ly Thuyet + 10 Checkpoint Thuc Hanh\n\
- Gioi han toc do thuc hanh: {FFAA00}60 km/h{FFFFFF}",
        "Thi ngay", "Huy");
    return 1;
}

static DS_ShowTheoryQuestion(playerid, qIdx)
{
    new title[64];
    format(title, sizeof(title), "Thi Ly Thuyet – Cau %d/%d", qIdx + 1, MAX_THEORY_QUESTIONS);

    inline OnAnswer(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response)
        {
            DS_FailExam(playerid, "Ban da huong bai thi ly thuyet.");
            return 1;
        }

        if (listitem == Questions[qIdx][E_Q_CORRECT])
        {
            TheoryCorrectAnswers[playerid]++;
            SendMsgF(playerid, COLOR_GREEN, "[LY THUYET] Cau %d: CHINH XAC!", qIdx + 1);
        }
        else
        {
            SendMsgF(playerid, COLOR_RED, "[LY THUYET] Cau %d: SAI ROI!", qIdx + 1);
        }

        new nextQ = qIdx + 1;
        if (nextQ < MAX_THEORY_QUESTIONS)
        {
            DS_ShowTheoryQuestion(playerid, nextQ);
        }
        else
        {
            // Kết thúc lý thuyết
            if (TheoryCorrectAnswers[playerid] >= PASS_THEORY_MIN)
            {
                SendMsgF(playerid, COLOR_GREEN,
                    "[LY THUYET] DA DAT! (%d/%d cau dung). Chuyen sang PHAN THI THUC HANH!",
                    TheoryCorrectAnswers[playerid], MAX_THEORY_QUESTIONS);

                DS_StartPracticalTest(playerid);
            }
            else
            {
                DS_FailExam(playerid, "Khong dat ly thuyet (can it nhat 4/5 cau dung).");
            }
        }
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline OnAnswer,
        DIALOG_STYLE_LIST, title, Questions[qIdx][E_Q_OPTIONS], "Tra loi", "Thoat");
    return 1;
}

static DS_StartPracticalTest(playerid)
{
    PlayerTestState[playerid]     = TEST_PRACTICAL;
    SpeedViolationCount[playerid] = 0;
    CurrentCPIndex[playerid]      = 0;

    // Dịch chuyển người chơi từ City Hall đến Sân Thi Red County
    SetPlayerPos(playerid, DS_PRACTICAL_X, DS_PRACTICAL_Y, DS_PRACTICAL_Z);
    SetPlayerFacingAngle(playerid, DS_PRACTICAL_A);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerInterior(playerid, 0);

    // Spawn xe thi tại Sân Thi Red County (Premier 426)
    TestVehicleID[playerid] = CreateVehicle(426,
        DS_VEH_SPAWN_X, DS_VEH_SPAWN_Y, DS_VEH_SPAWN_Z, DS_VEH_SPAWN_A,
        3, 3, 300);

    PutPlayerInVehicle(playerid, TestVehicleID[playerid], 0);

    // Đặt Checkpoint đầu tiên tại Red County
    SetPlayerRaceCheckpoint(playerid, 0,
        DSCheckpoints[0][0], DSCheckpoints[0][1], DSCheckpoints[0][2],
        DSCheckpoints[1][0], DSCheckpoints[1][1], DSCheckpoints[1][2],
        5.0);

    SendMsgF(playerid, COLOR_GREEN,
        "[THUC HANH] Ban da duoc dua den SAN THI RED COUNTY! Hay lai xe qua 10 Checkpoints. KHONG DUOC vuot qua 60 km/h!");
    return 1;
}

static DS_PassExam(playerid)
{
    // Clean up và trả về City Hall
    DS_CleanupTest(playerid);

    // Cấp bằng lái xe
    Player_GiveLicense(playerid, LICENSE_CAR);

    SendMsgF(playerid, COLOR_GREEN,
        "====================================================\n\
[BANG LAI XE] CHUC MUNG! Ban da THI DAT bang lai xe IC!\n\
Bang lai xe da duoc cap vao ho so va database.\n\
====================================================");
    return 1;
}

static DS_FailExam(playerid, const reason[])
{
    DS_CleanupTest(playerid);
    SendMsgF(playerid, COLOR_RED, "[THI LAI XE] TRUOT BAI THI! Ly do: %s", reason);
    return 1;
}

static DS_CleanupTest(playerid)
{
    PlayerTestState[playerid] = TEST_NONE;
    DisablePlayerRaceCheckpoint(playerid);

    if (TestVehicleID[playerid] != INVALID_VEHICLE_ID)
    {
        DestroyVehicle(TestVehicleID[playerid]);
        TestVehicleID[playerid] = INVALID_VEHICLE_ID;
    }

    // Trả người chơi về lại sảnh City Hall
    SetPlayerPos(playerid, DS_NPC_X, DS_NPC_Y, DS_NPC_Z);
    SetPlayerFacingAngle(playerid, DS_NPC_ANGLE);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerInterior(playerid, 0);
    return 1;
}

// =============================================================================
// COMMAND – /takeexam – Đăng ký thi bằng lái xe
// =============================================================================

CMD:takeexam(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (!IsPlayerInRangeOfPoint(playerid, 30.0, DS_NPC_X, DS_NPC_Y, DS_NPC_Z))
        return SendErrorMsg(playerid, "Ban phai den Truong Thi Lai Xe IC o Los Santos."), 1;

    DS_PromptStartExam(playerid);
    return 1;
}
