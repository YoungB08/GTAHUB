/*
    ui_phone.inc
    ------------
    Smartphone TextDraw HUD – Giao diện điện thoại góc phải trên màn hình.

    Thiết kế:
    - Góc phải trên: Mini phone UI hiện thông tin thời gian, số điện thoại, cuộc gọi đến.
    - Bình thường: Chỉ hiện icon pin nhỏ + số DT.
    - Khi có cuộc gọi đến: Flashin "INCOMING CALL" banner.
    - Khi đang nghe: "ON CALL – xx:xx" timer.
    - Dùng PlayerTextDraw để không ảnh hưởng player khác.

    Vị trí: X=450.0, Y=10.0 (góc phải trên, cạnh XP bar).
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS – Vị trí UI
// =============================================================================

// Nền điện thoại
#define PHONE_UI_BG_X       (480.0)
#define PHONE_UI_BG_Y       (6.0)
#define PHONE_UI_BG_W       (90.0)
#define PHONE_UI_BG_H       (28.0)
#define PHONE_UI_BG_COLOR   (0x00000099)  // Đen bán trong suốt

// Text số điện thoại
#define PHONE_NUM_TEXT_X    (483.0)
#define PHONE_NUM_TEXT_Y    (9.0)

// Icon cuộc gọi đến (sẽ nhấp nháy)
#define PHONE_CALL_TEXT_X   (483.0)
#define PHONE_CALL_TEXT_Y   (22.0)

// Update interval (ms)
#define PHONE_UI_UPDATE_MS  (1000)

// =============================================================================
// DATA
// =============================================================================

enum E_PHONE_UI {
    PlayerText:phoneBackground,
    PlayerText:phoneNumber,
    PlayerText:phoneCallStatus
}

static PhoneUI[MAX_PLAYERS][E_PHONE_UI];
static bool:PhoneUIVisible[MAX_PLAYERS];

// Trạng thái cuộc gọi hiển thị
enum E_PHONE_CALL_STATE { PHONE_IDLE, PHONE_RINGING, PHONE_ON_CALL }
static PhoneCallState[MAX_PLAYERS];
static PhoneCallSeconds[MAX_PLAYERS]; // Giây đã nói chuyện

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid)
{
    PhoneUIVisible[playerid] = false;
    PhoneCallState[playerid] = PHONE_IDLE;
    PhoneCallSeconds[playerid] = 0;
    return 1;
}

hook OnPlayerSpawn(playerid)
{
    if (Player_IsLoggedIn(playerid))
        Phone_UI_Show(playerid);
    return 1;
}

hook OnPlayerLogin(playerid)
{
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    Phone_UI_Hide(playerid);
    return 1;
}

// =============================================================================
// INIT / DESTROY
// =============================================================================

static Phone_UI_Create(playerid)
{
    // Nền (box)
    PhoneUI[playerid][phoneBackground] = CreatePlayerTextDraw(playerid,
        PHONE_UI_BG_X, PHONE_UI_BG_Y, "_");
    PlayerTextDrawFont(playerid, PhoneUI[playerid][phoneBackground], TEXT_DRAW_FONT_PREVIEW_MODEL);
    PlayerTextDrawTextSize(playerid, PhoneUI[playerid][phoneBackground],
        PHONE_UI_BG_X + PHONE_UI_BG_W, PHONE_UI_BG_Y + PHONE_UI_BG_H);
    PlayerTextDrawColor(playerid, PhoneUI[playerid][phoneBackground], COLOR_WHITE);
    PlayerTextDrawBoxColor(playerid, PhoneUI[playerid][phoneBackground], PHONE_UI_BG_COLOR);
    PlayerTextDrawUseBox(playerid, PhoneUI[playerid][phoneBackground], 1);

    // Số điện thoại
    PhoneUI[playerid][phoneNumber] = CreatePlayerTextDraw(playerid,
        PHONE_NUM_TEXT_X, PHONE_NUM_TEXT_Y, "No Phone");
    PlayerTextDrawFont(playerid, PhoneUI[playerid][phoneNumber], TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, PhoneUI[playerid][phoneNumber], 0.17, 0.8);
    PlayerTextDrawColor(playerid, PhoneUI[playerid][phoneNumber], 0x00FF80FF); // Xanh lá
    PlayerTextDrawSetShadow(playerid, PhoneUI[playerid][phoneNumber], 0);
    PlayerTextDrawSetOutline(playerid, PhoneUI[playerid][phoneNumber], 1);

    // Trạng thái cuộc gọi
    PhoneUI[playerid][phoneCallStatus] = CreatePlayerTextDraw(playerid,
        PHONE_CALL_TEXT_X, PHONE_CALL_TEXT_Y, " ");
    PlayerTextDrawFont(playerid, PhoneUI[playerid][phoneCallStatus], TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, PhoneUI[playerid][phoneCallStatus], 0.14, 0.65);
    PlayerTextDrawColor(playerid, PhoneUI[playerid][phoneCallStatus], 0xFFFFFFFF);
    PlayerTextDrawSetShadow(playerid, PhoneUI[playerid][phoneCallStatus], 0);
    PlayerTextDrawSetOutline(playerid, PhoneUI[playerid][phoneCallStatus], 1);
}

static Phone_UI_Destroy(playerid)
{
    if (PhoneUI[playerid][phoneBackground] != PlayerText:INVALID_TEXT_DRAW)
        DestroyPlayerTextDraw(playerid, PhoneUI[playerid][phoneBackground]);
    if (PhoneUI[playerid][phoneNumber] != PlayerText:INVALID_TEXT_DRAW)
        DestroyPlayerTextDraw(playerid, PhoneUI[playerid][phoneNumber]);
    if (PhoneUI[playerid][phoneCallStatus] != PlayerText:INVALID_TEXT_DRAW)
        DestroyPlayerTextDraw(playerid, PhoneUI[playerid][phoneCallStatus]);
}

// =============================================================================
// SHOW / HIDE / UPDATE
// =============================================================================

stock Phone_UI_Show(playerid)
{
    if (!PhoneUIVisible[playerid])
    {
        Phone_UI_Create(playerid);
        PhoneUIVisible[playerid] = true;
    }

    // Cập nhật nội dung
    Phone_UI_Update(playerid);

    // Hiển thị
    PlayerTextDrawShow(playerid, PhoneUI[playerid][phoneBackground]);
    PlayerTextDrawShow(playerid, PhoneUI[playerid][phoneNumber]);
    PlayerTextDrawShow(playerid, PhoneUI[playerid][phoneCallStatus]);
}

stock Phone_UI_Hide(playerid)
{
    if (PhoneUIVisible[playerid])
    {
        PlayerTextDrawHide(playerid, PhoneUI[playerid][phoneBackground]);
        PlayerTextDrawHide(playerid, PhoneUI[playerid][phoneNumber]);
        PlayerTextDrawHide(playerid, PhoneUI[playerid][phoneCallStatus]);
        Phone_UI_Destroy(playerid);
        PhoneUIVisible[playerid] = false;
    }
}

stock Phone_UI_Update(playerid)
{
    if (!PhoneUIVisible[playerid]) return;

    // Cập nhật số điện thoại
    new numDisplay[20];
    if (!isnull(PhoneNumber[playerid]))
        format(numDisplay, sizeof(numDisplay), "~b~%s", PhoneNumber[playerid]);
    else
        format(numDisplay, sizeof(numDisplay), "~w~No SIM");

    PlayerTextDrawSetString(playerid, PhoneUI[playerid][phoneNumber], numDisplay);

    // Cập nhật trạng thái cuộc gọi
    new callStr[32];
    switch (PhoneCallState[playerid])
    {
        case PHONE_IDLE:     callStr = " ";
        case PHONE_RINGING:  callStr = "~r~... Cuoc goi den ...";
        case PHONE_ON_CALL:
        {
            new mins = PhoneCallSeconds[playerid] / 60;
            new secs = PhoneCallSeconds[playerid] % 60;
            format(callStr, sizeof(callStr), "~g~Dang goi %02d:%02d", mins, secs);
        }
    }
    PlayerTextDrawSetString(playerid, PhoneUI[playerid][phoneCallStatus], callStr);
}

// =============================================================================
// HOOKS – Cập nhật trạng thái mỗi giây
// =============================================================================

hook OnPlayerSecondUpdate(playerid)
{
    if (!PhoneUIVisible[playerid]) return 1;

    // Đếm giây cuộc gọi
    if (PhoneCallState[playerid] == PHONE_ON_CALL)
        PhoneCallSeconds[playerid]++;

    // Cập nhật UI mỗi giây
    Phone_UI_Update(playerid);

    return 1;
}

// =============================================================================
// PUBLIC API – Được gọi từ phone_system.inc
// =============================================================================

stock Phone_UI_SetRinging(playerid)
{
    PhoneCallState[playerid]   = PHONE_RINGING;
    PhoneCallSeconds[playerid] = 0;
    Phone_UI_Update(playerid);
}

stock Phone_UI_SetOnCall(playerid)
{
    PhoneCallState[playerid]   = PHONE_ON_CALL;
    PhoneCallSeconds[playerid] = 0;
    Phone_UI_Update(playerid);
}

stock Phone_UI_SetIdle(playerid)
{
    PhoneCallState[playerid]   = PHONE_IDLE;
    PhoneCallSeconds[playerid] = 0;
    Phone_UI_Update(playerid);
}
