/*
    driving_school.inc
    ------------------
    Hệ thống Trường Thi Lái Xe & Bài Kiểm Tra Thực Hành (Driving School RP).
    Toàn bộ hệ thống tập trung tại Red County với đường thi 11 Checkpoints.

    Flow:
    1. Người chơi đến NPC Trường thi lái xe (Red County).
    2. Đăng ký thi lái xe (/takeexam hoặc bấm Y tại NPC).
    3. Thi Lý Thuyết: Bộ 5 câu hỏi trắc nghiệm Dialog về Luật Giao Thông IC. (Đạt >=4/5 câu).
    4. Thi Thực Hành (Red County - 11 Checkpoints):
       - Cấp xe thi (Bike: PCJ-600, Car: Premier, Truck: Flatbed).
       - Chạy qua 11 Checkpoints định sẵn tại Red County.
       - Giới hạn tốc độ: 60 km/h (vượt quá 3 lần bị hủy bài thi).
       - Hỏng xe quá 25% -> Trượt bài thi.
    5. Đạt yêu cầu -> Tự động cấp Bằng lái xe tương ứng lưu DB.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define DRIVING_SCHOOL_FEE       (500)      // Phí thi bằng lái
#define MAX_THEORY_QUESTIONS     (5)        // Số câu hỏi lý thuyết
#define PASS_THEORY_MIN          (4)        // Số câu đúng tối thiểu
#define MAX_SPEED_VIOLATIONS     (3)        // Tối đa 3 lần vượt 60km/h
#define MAX_TEST_CHECKPOINTS     (11)       // 11 Checkpoints thực hành tại Red County
#define TEST_SPEED_LIMIT         (60.0)     // km/h

// Vị trí Trung Tâm Đăng Ký Thi Bằng Lái Duy Nhất (Red County)
#define DS_NPC_X                 (854.5622)
#define DS_NPC_Y                 (-604.4556)
#define DS_NPC_Z                 (18.4219)
#define DS_NPC_ANGLE             (358.2418)

// Vị trí spawn xe thi thực hành tại Red County
#define DS_VEH_SPAWN_X           (849.7348)
#define DS_VEH_SPAWN_Y           (-560.9124)
#define DS_VEH_SPAWN_Z           (16.9223)
#define DS_VEH_SPAWN_A           (101.0128)

// 11 Checkpoints bài thi thực hành duy nhất tại Red County
static const Float:DSCheckpoints[MAX_TEST_CHECKPOINTS][3] = {
    {773.8661, -525.8478, 16.1875},
    {702.6376, -597.2467, 16.1875},
    {679.2297, -713.8463, 16.0839},
    {707.5011, -868.0165, 43.2667},
    {792.4102, -1031.7183, 25.0062},
    {793.5915, -1382.2426, 13.4912},
    {1122.1737, -1403.3391, 13.4068},
    {1409.7452, -1443.3870, 13.3828},
    {1427.7283, -1577.2148, 13.3619},
    {1426.7699, -1735.8705, 13.3828},
    {1496.4657, -1738.6531, 13.5469}
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
static SelectedLicenseType[MAX_PLAYERS];
static TestScore[MAX_PLAYERS];
static Float:TestVehicleHealth[MAX_PLAYERS];
static TestDeadline[MAX_PLAYERS];

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
        "1. Tang toc qua nhanh\n2. Dung lai truoc vach qua duong\n3. Re phai ngay khong can dung",
        1
    },
    {
        "Toc do toi da cho phep trong khu dan cu la bao nhieu?",
        "1. 60 km/h\n2. 100 km/h\n3. Khong gioi han",
        0
    },
    {
        "Khi nghe tieng coi xe Canh Sat / EMS dang cap cuu, ban phai:",
        "1. Dua xe voi ho\n2. Nhuong duong bang cach tap vao le phai\n3. Dung lai giua duong",
        1
    },
    {
        "Lai xe khi dang say ruou / ngao ma tuy se bi xu ly nhu the nao?",
        "1. Khong sao ca\n2. Bi phat tien va tuoc bang lai IC\n3. Duoc thuong tien",
        1
    },
    {
        "Khi chuyen huong hoac re duong, ban bat buoc phai:",
        "1. Bat den xi-nhan bao hieu truoc\n2. Bop coi lien tuc\n3. Khong can lam gi",
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
    // Tạo 1 NPC Giảng viên thi lái xe duy nhất tại Red County
    Create3DTextLabel(
        "Truong Thi Lai Xe IC (Red County)\n"C_YELLOW"Nhan Y hoac /takeexam de dang ky thi",
        COLOR_WHITE, DS_NPC_X, DS_NPC_Y, DS_NPC_Z + 1.0, 30.0, 0, false);

    new actor = CreateActor(17, DS_NPC_X, DS_NPC_Y, DS_NPC_Z, DS_NPC_ANGLE);
    SetActorVirtualWorld(actor, 0);

    Logger_Log("[driving_school.inc] Driving school initialized at Red County.");
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if (PRESSED(KEY_YES))
    {
        if (IsPlayerInRangeOfPoint(playerid, 4.0, DS_NPC_X, DS_NPC_Y, DS_NPC_Z))
        {
            CallLocalFunction("cmd_takeexam", "is", playerid, "\1");
        }
    }
    return 1;
}

hook OnPlayerEnterRaceCheckpoint(playerid)
{
    if (PlayerTestState[playerid] != TEST_PRACTICAL) return 1;

    new nextCP = CurrentCPIndex[playerid] + 1;
    new maxCPs = MAX_TEST_CHECKPOINTS;

    if (nextCP >= maxCPs)
    {
        // Hoàn thành xuất sắc bài thi
        DS_PassExam(playerid);
    }
    else
    {
        CurrentCPIndex[playerid] = nextCP;
        DisablePlayerRaceCheckpoint(playerid);

        new type = (nextCP == maxCPs - 1) ? 1 : 0; // 1 = Finish flag
        SetPlayerRaceCheckpoint(playerid, type,
            DSCheckpoints[nextCP][0], DSCheckpoints[nextCP][1], DSCheckpoints[nextCP][2],
            (nextCP == maxCPs - 1) ? DSCheckpoints[nextCP][0] : DSCheckpoints[nextCP + 1][0],
            (nextCP == maxCPs - 1) ? DSCheckpoints[nextCP][1] : DSCheckpoints[nextCP + 1][1],
            (nextCP == maxCPs - 1) ? DSCheckpoints[nextCP][2] : DSCheckpoints[nextCP + 1][2],
            6.0);

        SendMsgF(playerid, COLOR_GREEN,
            "[THI LAI XE] Checkpoint %d/%d! Duy tri toc do duoi 60 km/h.",
            nextCP + 1, maxCPs);
    }

    return 1;
}

// Kiểm tra tốc độ thực hành, va chạm và thời gian mỗi giây
hook OnPlayerSecondUpdate(playerid)
{
    if (PlayerTestState[playerid] != TEST_PRACTICAL) return 1;

    if (!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleID(playerid) != TestVehicleID[playerid])
    {
        DS_FailExam(playerid, "Ban da roi khoi xe thi!");
        return 1;
    }

    // Kiểm tra thời gian tối đa bài thi (4 phút)
    if (gettime() > TestDeadline[playerid])
    {
        DS_FailExam(playerid, "Qua thoi gian thi (Toi da 4 phut).");
        return 1;
    }

    // Kiểm tra va chạm xe và trừ điểm
    new Float:health;
    GetVehicleHealth(TestVehicleID[playerid], health);
    if (health + 25.0 < TestVehicleHealth[playerid])
    {
        TestScore[playerid] -= 10;
        TestVehicleHealth[playerid] = health;
        SendMsgF(playerid, COLOR_RED, "[CANH BAO] Ban bi tru 10 diem do va cham xe! Diem hien tai: %d/100 (Yeu cau >= 70)", TestScore[playerid]);

        if (TestScore[playerid] < 70)
        {
            DS_FailExam(playerid, "Va cham qua nhieu (Diem so duoi 70).");
            return 1;
        }
    }

    // Kiem tra toc do xe (dong bo voi dong ho Speedometer, giam 1/3 giup thi de hon)
    new Float:speed = GetVehicleSpeed(TestVehicleID[playerid]);

    if (speed > TEST_SPEED_LIMIT)
    {
        SpeedViolationCount[playerid]++;
        SendMsgF(playerid, COLOR_RED,
            "[CANH BAO] Toc do: %.1f km/h (Vuot qua 60 km/h)! Vi pham: %d/%d",
            speed, SpeedViolationCount[playerid], MAX_SPEED_VIOLATIONS);

        if (SpeedViolationCount[playerid] >= MAX_SPEED_VIOLATIONS)
        {
            DS_FailExam(playerid, "Vuot qua toc do 60 km/h qua 3 lan.");
            return 1;
        }
    }

    return 1;
}

// =============================================================================
// EXAM FLOW
// =============================================================================

static DS_PromptStartExam(playerid, licenseType, fee, const licenseName[])
{
    if (Player_HasLicense(playerid, licenseType))
        return SendErrorMsgF(playerid, "Ban da co %s roi! Khong can thi lai.", licenseName), 1;

    if (PlayerTestState[playerid] != TEST_NONE)
        return SendErrorMsg(playerid, "Ban dang trong bai thi roi."), 1;

    if (GetPlayerMoney(playerid) < fee)
        return SendErrorMsgF(playerid, "Can $%d de dang ky thi %s.", fee, licenseName), 1;

    SelectedLicenseType[playerid] = licenseType;

    inline OnConfirm(response, listitem, string:inputtext[])
    {
        #pragma unused listitem, inputtext
        if (!response) return 1;

        GivePlayerMoney(playerid, -fee);

        // Bắt đầu phần thi Lý thuyết
        PlayerTestState[playerid]      = TEST_THEORY;
        TheoryCorrectAnswers[playerid] = 0;
        TheoryCurrentQ[playerid]       = 0;

        SendMsgF(playerid, COLOR_GREEN,
            "[THI LAI XE] Da dong phi $%d. Bat dau phan thi LY THUYET %s (5 cau hoi).", fee, licenseName);

        DS_ShowTheoryQuestion(playerid, 0);
        return 1;
    }

    new msg[256];
    format(msg, sizeof(msg),
        "Ban co muon dang ky thi %s?\n\n\
- Phi thi: {33AA33}$%d{FFFFFF}\n\
- Bai thi bao gom: 5 cau Ly Thuyet + 11 Checkpoint Thuc Hanh Red County\n\
- Gioi han toc do thuc hanh: {FFAA00}60 km/h{FFFFFF}", licenseName, fee);

    Dialog_ShowCallback(playerid, using inline OnConfirm,
        DIALOG_STYLE_MSGBOX,
        "Dang Ky Thi Bang Lai Xe IC",
        msg,
        "Thi ngay", "Huy");
    return 1;
}

static DS_ShowTheoryQuestion(playerid, qIdx)
{
    new title[128];
    format(title, sizeof(title), "Cau %d/%d: %s", qIdx + 1, MAX_THEORY_QUESTIONS, Questions[qIdx][E_Q_TEXT]);

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
                    "[LY THUYET] DA DAT! (%d/%d cau dung). Chuyen sang PHAN THI THUC HANH AT RED COUNTY!",
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
    TestScore[playerid]           = 100;
    TestDeadline[playerid]        = gettime() + 240;

    SetPlayerPos(playerid, DS_VEH_SPAWN_X, DS_VEH_SPAWN_Y, DS_VEH_SPAWN_Z);
    SetPlayerFacingAngle(playerid, DS_VEH_SPAWN_A);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerInterior(playerid, 0);

    new vehModel = 426; // Premier (Car)
    new constName[64] = "O TO (CAR)";

    if (SelectedLicenseType[playerid] == LICENSE_TRUCK)
    {
        vehModel = 455; // Flatbed (Truck)
        strcpy(constName, "XE TAI (TRUCK / BUS)", sizeof(constName));
    }
    else if (SelectedLicenseType[playerid] == LICENSE_BIKE)
    {
        vehModel = 461; // PCJ-600 (Bike)
        strcpy(constName, "XE MAY (BIKE)", sizeof(constName));
    }

    TestVehicleID[playerid] = CreateVehicle(vehModel,
        DS_VEH_SPAWN_X, DS_VEH_SPAWN_Y, DS_VEH_SPAWN_Z, DS_VEH_SPAWN_A,
        3, 3, 300);

    GetVehicleHealth(TestVehicleID[playerid], TestVehicleHealth[playerid]);

    // Cấp quyền lái xe tạm thời cho đúng phương tiện thi này
    SetPVarInt(playerid, "LicenseTestActive", 1);
    SetPVarInt(playerid, "LicenseTestVehID", TestVehicleID[playerid]);

    PutPlayerInVehicle(playerid, TestVehicleID[playerid], 0);

    // Đặt Checkpoint đầu tiên cho bài thi Red County
    SetPlayerRaceCheckpoint(playerid, 0,
        DSCheckpoints[0][0], DSCheckpoints[0][1], DSCheckpoints[0][2],
        DSCheckpoints[1][0], DSCheckpoints[1][1], DSCheckpoints[1][2],
        6.0);

    SendMsgF(playerid, COLOR_GREEN,
        "[THUC HANH RED COUNTY] Dang thi BANG LAI %s! Hay lai xe qua 11 Checkpoints. Toc do < 60 km/h!", constName);
    return 1;
}

static DS_PassExam(playerid)
{
    new licenseType = SelectedLicenseType[playerid];

    // Clean up và trả về vị trí đăng ký Red County
    DS_CleanupTest(playerid);

    // Cấp bằng lái xe tương ứng
    Player_GiveLicense(playerid, licenseType);

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

    // Xóa quyền lái xe tạm thời
    DeletePVar(playerid, "LicenseTestActive");
    DeletePVar(playerid, "LicenseTestVehID");

    if (TestVehicleID[playerid] != INVALID_VEHICLE_ID)
    {
        DestroyVehicle(TestVehicleID[playerid]);
        TestVehicleID[playerid] = INVALID_VEHICLE_ID;
    }

    // Trả người chơi về lại điểm đăng ký Red County
    SetPlayerPos(playerid, DS_NPC_X, DS_NPC_Y, DS_NPC_Z);
    SetPlayerFacingAngle(playerid, DS_NPC_ANGLE);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerInterior(playerid, 0);
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if (PlayerTestState[playerid] != TEST_NONE)
    {
        DS_CleanupTest(playerid);
    }
    return 1;
}

hook OnPlayerDeath(playerid, killerid, reason)
{
    if (PlayerTestState[playerid] != TEST_NONE)
    {
        DS_FailExam(playerid, "Nguoi choi bi chet trong khi dang thi.");
    }
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
        return SendErrorMsg(playerid, "Ban phai den Trung Tam Thi Bang Lai (Red County) de dang ky."), 1;

    if (PlayerTestState[playerid] != TEST_NONE)
        return SendErrorMsg(playerid, "Ban dang trong bai thi roi."), 1;

    inline OnLicenseSelect(pid, dialogid, response, listitem, string:inputtext[])
    {
        #pragma unused pid, dialogid, inputtext
        if (!response) return 1;

        switch (listitem)
        {
            case 0: // Bang Lai Xe Mo To / Xe May (Bike) - $300
            {
                DS_PromptStartExam(playerid, LICENSE_BIKE, 300, "Bang Lai Xe Mo To / Xe May (Bike)");
            }
            case 1: // Bang Lai Xe O To (Car) - $500
            {
                DS_PromptStartExam(playerid, LICENSE_CAR, 500, "Bang Lai Xe O To (Car)");
            }
            case 2: // Bang Lai Xe Tai / Bus (Truck) - $1,000
            {
                DS_PromptStartExam(playerid, LICENSE_TRUCK, 1000, "Bang Lai Xe Tai (Truck / Bus)");
            }
        }
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline OnLicenseSelect,
        DIALOG_STYLE_LIST,
        "=== CHON LOAI BANG LAI XE THI ===",
        "1. Bang Lai Xe Mo To / Xe May (Bike) - $300\n\
2. Bang Lai Xe O To (Car) - $500\n\
3. Bang Lai Xe Tai / Bus (Truck) - $1,000",
        "Chon", "Huy");
    return 1;
}
