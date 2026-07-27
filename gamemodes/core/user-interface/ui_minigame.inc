/*
* Reusable Minigame UI System
*
* Description:
*   Centralized UI system for all minigames. Creates textdraws and progress bars once,
*   then reuses them for all minigames to save memory and resources.
*
* Features:
*   - Single progress bar/textdraw set per player (reused for all minigames)
*   - Customizable colors, fonts, sizes for each show
*   - Hide/show UI elements on demand
*   - Support for title, instructions, and progress information
*   - Clean API for minigame systems
*
* Functions:
*   MinigameUI_Initialize(playerid)
*       - Initialize UI resources for a player (called on connect)
*   MinigameUI_Show(playerid, title, instructions, color = -1)
*       - Show minigame UI with custom title and instructions
*   MinigameUI_Hide(playerid)
*       - Hide all minigame UI elements
*   MinigameUI_SetProgress(playerid, Float:progress)
*       - Update progress bar (0.0 - 100.0)
*   MinigameUI_SetProgressColor(playerid, color)
*       - Change progress bar color
*   MinigameUI_SetTextColor(playerid, color)
*       - Change title/instructions text color
*   MinigameUI_Destroy(playerid)
*       - Destroy all UI resources for a player (called on disconnect)
*
* Example Usage:
*
*   // In your minigame start function:
*   MinigameUI_Show(playerid, "[HOLD KEY]", "Hold SPACE to complete", 0xFF0000FF);
*
*   // In your progress update:
*   MinigameUI_SetProgress(playerid, progress);
*
*   // When done:
*   MinigameUI_Hide(playerid);
*
* By s4marium
*/

#if defined _inc_ui_minigame
    #undef _inc_ui_minigame
#endif

#if defined _ui_minigame_included
    #endinput
#endif
#define _ui_minigame_included

#include <YSI_Coding\y_hooks>

// Color presets
#define MINIGAME_COLOR_DEFAULT          (-1)
#define MINIGAME_COLOR_SUCCESS          (0x00FF00FF)
#define MINIGAME_COLOR_WARNING          (0xFFAA00FF)
#define MINIGAME_COLOR_DANGER           (0xFF0000FF)
#define MINIGAME_COLOR_INFO             (0x00FFFFFF)

// UI position presets
#define MINIGAME_UI_POS_CENTER_X        (272.0)
#define MINIGAME_UI_POS_CENTER_Y        (363.0)
#define MINIGAME_UI_BAR_WIDTH           (107.5)
#define MINIGAME_UI_BAR_HEIGHT          (9.2)

// UI configuration
#if !defined MINIGAME_UI_MAX_PLAYERS
    #define MINIGAME_UI_MAX_PLAYERS MAX_PLAYERS
#endif

enum E_MINIGAME_UI
{
    // Textdraws
    PlayerText:mg_Title,
    PlayerText:mg_Instructions,
    PlayerBar:mg_ProgressBar,

    // State
    bool:mg_Visible,
    mg_TitleColor,
    mg_ProgressColor,
};

static s_MinigameUI[MINIGAME_UI_MAX_PLAYERS][E_MINIGAME_UI];

// Initialize UI for a player
stock MinigameUI_Initialize(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    // Skip if already initialized
    if (s_MinigameUI[playerid][mg_Title] != PlayerText:INVALID_TEXT_DRAW)
        return 1;

    // Initialize all handles to INVALID before creation
    s_MinigameUI[playerid][mg_Title] = PlayerText:INVALID_TEXT_DRAW;
    s_MinigameUI[playerid][mg_Instructions] = PlayerText:INVALID_TEXT_DRAW;
    s_MinigameUI[playerid][mg_ProgressBar] = PlayerBar:INVALID_PLAYER_BAR_ID;
    s_MinigameUI[playerid][mg_Visible] = false;
    s_MinigameUI[playerid][mg_TitleColor] = -1;
    s_MinigameUI[playerid][mg_ProgressColor] = -1;

    // Create progress bar
    s_MinigameUI[playerid][mg_ProgressBar] = CreatePlayerProgressBar(
        playerid,
        MINIGAME_UI_POS_CENTER_X,
        MINIGAME_UI_POS_CENTER_Y,
        MINIGAME_UI_BAR_WIDTH,
        MINIGAME_UI_BAR_HEIGHT,
        -1,
        100.0,
        0
    );

    // Create title textdraw
    s_MinigameUI[playerid][mg_Title] = CreatePlayerTextDraw(playerid, 270.0, 350.0, "[MINIGAME]");
    PlayerTextDrawBackgroundColor(playerid, s_MinigameUI[playerid][mg_Title], 255);
    PlayerTextDrawFont(playerid, s_MinigameUI[playerid][mg_Title], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, s_MinigameUI[playerid][mg_Title], 0.249999, 1.100000);
    PlayerTextDrawColor(playerid, s_MinigameUI[playerid][mg_Title], -1);
    PlayerTextDrawSetOutline(playerid, s_MinigameUI[playerid][mg_Title], 0);
    PlayerTextDrawSetProportional(playerid, s_MinigameUI[playerid][mg_Title], true);
    PlayerTextDrawSetShadow(playerid, s_MinigameUI[playerid][mg_Title], 1);
    PlayerTextDrawSetSelectable(playerid, s_MinigameUI[playerid][mg_Title], false);

    // Create instructions textdraw
    s_MinigameUI[playerid][mg_Instructions] = CreatePlayerTextDraw(playerid, 270.0, 375.0, "Press SPACE to start");
    PlayerTextDrawBackgroundColor(playerid, s_MinigameUI[playerid][mg_Instructions], 255);
    PlayerTextDrawFont(playerid, s_MinigameUI[playerid][mg_Instructions], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, s_MinigameUI[playerid][mg_Instructions], 0.249999, 1.000000);
    PlayerTextDrawColor(playerid, s_MinigameUI[playerid][mg_Instructions], -1);
    PlayerTextDrawSetOutline(playerid, s_MinigameUI[playerid][mg_Instructions], 0);
    PlayerTextDrawSetProportional(playerid, s_MinigameUI[playerid][mg_Instructions], true);
    PlayerTextDrawSetShadow(playerid, s_MinigameUI[playerid][mg_Instructions], 1);
    PlayerTextDrawSetSelectable(playerid, s_MinigameUI[playerid][mg_Instructions], false);

    return 1;
}

// Show minigame UI
stock MinigameUI_Show(playerid, const title[], const instructions[], color = -1)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (s_MinigameUI[playerid][mg_Title] == PlayerText:INVALID_TEXT_DRAW)
        return 0;

    // Update title and instructions text (no color codes - use PlayerTextDrawColor)
    PlayerTextDrawSetString(playerid, s_MinigameUI[playerid][mg_Title], title);
    PlayerTextDrawSetString(playerid, s_MinigameUI[playerid][mg_Instructions], instructions);

    // Set color if provided
    if (color != -1)
    {
        s_MinigameUI[playerid][mg_TitleColor] = color;
        PlayerTextDrawColor(playerid, s_MinigameUI[playerid][mg_Title], color);
    }

    // Show UI elements
    PlayerTextDrawShow(playerid, s_MinigameUI[playerid][mg_Title]);
    PlayerTextDrawShow(playerid, s_MinigameUI[playerid][mg_Instructions]);
    ShowPlayerProgressBar(playerid, s_MinigameUI[playerid][mg_ProgressBar]);
    SetPlayerProgressBarValue(playerid, s_MinigameUI[playerid][mg_ProgressBar], 0.0);

    s_MinigameUI[playerid][mg_Visible] = true;

    return 1;
}

// Hide minigame UI
stock MinigameUI_Hide(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (s_MinigameUI[playerid][mg_Title] == PlayerText:INVALID_TEXT_DRAW)
        return 0;

    // Hide all UI elements
    PlayerTextDrawHide(playerid, s_MinigameUI[playerid][mg_Title]);
    PlayerTextDrawHide(playerid, s_MinigameUI[playerid][mg_Instructions]);
    HidePlayerProgressBar(playerid, s_MinigameUI[playerid][mg_ProgressBar]);
    SetPlayerProgressBarValue(playerid, s_MinigameUI[playerid][mg_ProgressBar], 0.0);

    // Reset color to default
    PlayerTextDrawColor(playerid, s_MinigameUI[playerid][mg_Title], -1);
    PlayerTextDrawColor(playerid, s_MinigameUI[playerid][mg_Instructions], -1);

    s_MinigameUI[playerid][mg_Visible] = false;

    return 1;
}

// Set progress bar value
stock MinigameUI_SetProgress(playerid, Float:progress)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (!s_MinigameUI[playerid][mg_Visible])
        return 0;

    // Clamp progress between 0.0 and 100.0
    if (progress < 0.0)
        progress = 0.0;
    else if (progress > 100.0)
        progress = 100.0;

    SetPlayerProgressBarValue(playerid, s_MinigameUI[playerid][mg_ProgressBar], progress);

    return 1;
}

// Set progress bar color
stock MinigameUI_SetProgressColor(playerid, color)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (s_MinigameUI[playerid][mg_ProgressBar] == PlayerBar:INVALID_PLAYER_BAR_ID)
        return 0;

    SetPlayerProgressBarColour(playerid, s_MinigameUI[playerid][mg_ProgressBar], color);
    s_MinigameUI[playerid][mg_ProgressColor] = color;

    return 1;
}

// Set title and instructions color
stock MinigameUI_SetTextColor(playerid, color)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (s_MinigameUI[playerid][mg_Title] == PlayerText:INVALID_TEXT_DRAW)
        return 0;

    PlayerTextDrawColor(playerid, s_MinigameUI[playerid][mg_Title], color);
    PlayerTextDrawColor(playerid, s_MinigameUI[playerid][mg_Instructions], color);
    s_MinigameUI[playerid][mg_TitleColor] = color;

    return 1;
}

// Update title text
stock MinigameUI_SetTitle(playerid, const title[])
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (s_MinigameUI[playerid][mg_Title] == PlayerText:INVALID_TEXT_DRAW)
        return 0;

    PlayerTextDrawSetString(playerid, s_MinigameUI[playerid][mg_Title], title);

    return 1;
}

// Update instructions text
stock MinigameUI_SetInstructions(playerid, const instructions[])
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (s_MinigameUI[playerid][mg_Instructions] == PlayerText:INVALID_TEXT_DRAW)
        return 0;

    PlayerTextDrawSetString(playerid, s_MinigameUI[playerid][mg_Instructions], instructions);

    return 1;
}

// Check if UI is visible
stock bool:MinigameUI_IsVisible(playerid)
{
    if (!IsPlayerConnected(playerid))
        return false;

    return s_MinigameUI[playerid][mg_Visible];
}

// Destroy all UI resources for a player
stock MinigameUI_Destroy(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (s_MinigameUI[playerid][mg_Title] != PlayerText:INVALID_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, s_MinigameUI[playerid][mg_Title]);
        s_MinigameUI[playerid][mg_Title] = PlayerText:INVALID_TEXT_DRAW;
    }

    if (s_MinigameUI[playerid][mg_Instructions] != PlayerText:INVALID_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, s_MinigameUI[playerid][mg_Instructions]);
        s_MinigameUI[playerid][mg_Instructions] = PlayerText:INVALID_TEXT_DRAW;
    }

    if (s_MinigameUI[playerid][mg_ProgressBar] != PlayerBar:INVALID_PLAYER_BAR_ID)
    {
        DestroyPlayerProgressBar(playerid, s_MinigameUI[playerid][mg_ProgressBar]);
        s_MinigameUI[playerid][mg_ProgressBar] = PlayerBar:INVALID_PLAYER_BAR_ID;
    }

    s_MinigameUI[playerid][mg_Visible] = false;

    return 1;
}

// Hook to initialize UI when player connects
hook OnPlayerConnect(playerid)
{
    MinigameUI_Initialize(playerid);
    return 1;
}

// Hook to destroy UI when player disconnects
hook OnPlayerDisconnect(playerid, reason)
{
    MinigameUI_Destroy(playerid);
    return 1;
}
