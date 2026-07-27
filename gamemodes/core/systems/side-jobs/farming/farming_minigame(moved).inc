#include <YSI_Coding\y_hooks>

// ====== MINI-GAME CONSTANTS ======
#define MINIGAME_PROGRESS_MAX 100
#define MINIGAME_PROGRESS_INCREMENT 5
#define MINIGAME_PROGRESS_DECAY 10
#define MINIGAME_KEY_COOLDOWN 150 // Milliseconds between key presses
#define MINIGAME_DECAY_INTERVAL 1000 // Milliseconds between decay ticks
#define MINIGAME_MAX_TIME 60000 // 60 seconds max
#define MINIGAME_ANIMATION_INTERVAL 1000 // 1 second between animation loops

// ====== MINI-GAME TYPES ======
#define MINIGAME_TYPE_HARVEST 1
#define MINIGAME_TYPE_COLLECTION 2
#define MINIGAME_TYPE_MINING 3

// ====== MINI-GAME ENUMS ======
enum E_MINIGAME_STATE {
     bool:IsActive,
     PlayerID,
     Type,
     CustomData,
     Progress,
     LastKeyTime,
     DecayTimer,
     TimeoutTimer,
     AnimationTimer
};

// ====== MINI-GAME VARIABLES ======
new MiniGameState[MAX_PLAYERS][E_MINIGAME_STATE];

// Textdraw UI
new PlayerBar:MiniGameProgressBar[MAX_PLAYERS];
new PlayerText:MiniGameTitle[MAX_PLAYERS];
new PlayerText:MiniGameInstructions[MAX_PLAYERS];

// ====== FORWARD DECLARATIONS ======
forward public MiniGame_InitPlayer(playerid);
forward public MiniGame_OnKeyStateChange(playerid, newkeys, oldkeys);
forward public MiniGame_Start(playerid, type, customData, const title[]);
forward public MiniGame_End(playerid, bool:success);
forward public MiniGame_OnDecay(playerid);
forward public MiniGame_OnTimeout(playerid);
forward public MiniGame_LoopAnimation(playerid);

// ====== INITIALIZATION ======
hook OnGameModeInit() {
     LoadFarmingMinigame();
     return 1;
}

LoadFarmingMinigame() {
     print("[FARMING] Mini-game system loaded successfully!");
     return 1;
}

public MiniGame_InitPlayer(playerid) {
     MiniGameState[playerid][IsActive] = false;
     MiniGameState[playerid][Progress] = 0;
     MiniGameState[playerid][LastKeyTime] = 0;
     MiniGameState[playerid][DecayTimer] = 0;
     MiniGameState[playerid][TimeoutTimer] = 0;
     MiniGameState[playerid][AnimationTimer] = 0;

     // Create progress bar
     MiniGameProgressBar[playerid] = CreatePlayerProgressBar(playerid, 272.0, 363.0, 107.5, 9.2, -1347440641, 100.0, 0);

     // Create title textdraw
     MiniGameTitle[playerid] = CreatePlayerTextDraw(playerid, 270.0, 350.0, "[HARVEST]");
     PlayerTextDrawBackgroundColor(playerid, MiniGameTitle[playerid], 255);
     PlayerTextDrawFont(playerid, MiniGameTitle[playerid], TEXT_DRAW_FONT_2);
     PlayerTextDrawLetterSize(playerid, MiniGameTitle[playerid], 0.249999, 1.100000);
     PlayerTextDrawColor(playerid, MiniGameTitle[playerid], -1);
     PlayerTextDrawSetOutline(playerid, MiniGameTitle[playerid], 0);
     PlayerTextDrawSetProportional(playerid, MiniGameTitle[playerid], true);
     PlayerTextDrawSetShadow(playerid, MiniGameTitle[playerid], 1);
     PlayerTextDrawSetSelectable(playerid, MiniGameTitle[playerid], false);

     // Create instructions textdraw below progress bar
     MiniGameInstructions[playerid] = CreatePlayerTextDraw(playerid, 270.0, 375.0, "Press 'SPACE' to harvest");
     PlayerTextDrawBackgroundColor(playerid, MiniGameInstructions[playerid], 255);
     PlayerTextDrawFont(playerid, MiniGameInstructions[playerid], TEXT_DRAW_FONT_2);
     PlayerTextDrawLetterSize(playerid, MiniGameInstructions[playerid], 0.249999, 1.000000);
     PlayerTextDrawColor(playerid, MiniGameInstructions[playerid], -1);
     PlayerTextDrawSetOutline(playerid, MiniGameInstructions[playerid], 0);
     PlayerTextDrawSetProportional(playerid, MiniGameInstructions[playerid], true);
     PlayerTextDrawSetShadow(playerid, MiniGameInstructions[playerid], 1);
     PlayerTextDrawSetSelectable(playerid, MiniGameInstructions[playerid], false);

     return 1;
}

public MiniGame_OnKeyStateChange(playerid, newkeys, oldkeys) {
     if(!MiniGameState[playerid][IsActive]) return 0;

     // Check for SPACE key press
     if(PRESSED(KEY_SPRINT)) {
          new currentTime = GetTickCount();

          // Check key cooldown
          if(currentTime - MiniGameState[playerid][LastKeyTime] >= MINIGAME_KEY_COOLDOWN) {
               MiniGameState[playerid][LastKeyTime] = currentTime;
               MiniGame_UpdateProgress(playerid, MINIGAME_PROGRESS_INCREMENT);
          }
     }

     return 1;
}

// ====== PUBLIC FUNCTIONS ======
public MiniGame_Start(playerid, type, customData, const title[]) {
     if(MiniGameState[playerid][IsActive]) return 0;

     // Initialize state
     MiniGameState[playerid][IsActive] = true;
     MiniGameState[playerid][PlayerID] = playerid;
     MiniGameState[playerid][Type] = type;
     MiniGameState[playerid][CustomData] = customData;
     MiniGameState[playerid][Progress] = 0;
     MiniGameState[playerid][LastKeyTime] = GetTickCount();

     // Lock player
     TogglePlayerControllable(playerid, false);

     // Show UI (progress bar, title, and instructions)
     PlayerTextDrawSetString(playerid, MiniGameTitle[playerid], title);
     PlayerTextDrawShow(playerid, MiniGameTitle[playerid]);
     PlayerTextDrawShow(playerid, MiniGameInstructions[playerid]);
     ShowPlayerProgressBar(playerid, MiniGameProgressBar[playerid]);
     SetPlayerProgressBarValue(playerid, MiniGameProgressBar[playerid], 0.0);

     // Start decay timer
     MiniGameState[playerid][DecayTimer] = SetTimerEx("MiniGame_OnDecay", MINIGAME_DECAY_INTERVAL, true, "i", playerid);

     // Start timeout timer
     MiniGameState[playerid][TimeoutTimer] = SetTimerEx("MiniGame_OnTimeout", MINIGAME_MAX_TIME, false, "i", playerid);

     // Start animation loop for harvest type
     if(type == MINIGAME_TYPE_HARVEST) {
          MiniGameState[playerid][AnimationTimer] = SetTimerEx("MiniGame_LoopAnimation", MINIGAME_ANIMATION_INTERVAL, true, "i", playerid);
     }

     // Update display
     MiniGame_UpdateDisplay(playerid);

     return 1;
}

static MiniGame_UpdateProgress(playerid, amount) {
     if(!MiniGameState[playerid][IsActive]) return;

     MiniGameState[playerid][Progress] += amount;

     if(MiniGameState[playerid][Progress] >= MINIGAME_PROGRESS_MAX) {
          MiniGameState[playerid][Progress] = MINIGAME_PROGRESS_MAX;
          MiniGame_UpdateDisplay(playerid);
          MiniGame_Complete(playerid);
          return;
     }

     MiniGame_UpdateDisplay(playerid);
}

public MiniGame_OnDecay(playerid) {
     if(!MiniGameState[playerid][IsActive]) return 0;

     MiniGameState[playerid][Progress] -= MINIGAME_PROGRESS_DECAY;

     // Check if progress reached 0 (failure)
     if(MiniGameState[playerid][Progress] <= 0) {
          MiniGameState[playerid][Progress] = 0;
          MiniGame_UpdateDisplay(playerid);
          MiniGame_Fail(playerid);
          return 0;
     }

     MiniGame_UpdateDisplay(playerid);
     return 1;
}

static MiniGame_UpdateDisplay(playerid) {
     // Update progress bar value (0.0 to 100.0)
     new Float:barValue = float(MiniGameState[playerid][Progress]);
     SetPlayerProgressBarValue(playerid, MiniGameProgressBar[playerid], barValue);
}

static MiniGame_Complete(playerid) {
     MiniGame_End(playerid, true);
}

static MiniGame_Fail(playerid) {
     Alert_Send(playerid, "~r~ERROR:~w~ You were too slow! Collection failed.", 3000);
     MiniGame_End(playerid, false);
}

public MiniGame_End(playerid, bool:success) {
     if(!MiniGameState[playerid][IsActive]) return 0;

     // Kill timers
     if(MiniGameState[playerid][DecayTimer]) {
          KillTimer(MiniGameState[playerid][DecayTimer]);
          MiniGameState[playerid][DecayTimer] = 0;
     }
     if(MiniGameState[playerid][TimeoutTimer]) {
          KillTimer(MiniGameState[playerid][TimeoutTimer]);
          MiniGameState[playerid][TimeoutTimer] = 0;
     }
     if(MiniGameState[playerid][AnimationTimer]) {
          KillTimer(MiniGameState[playerid][AnimationTimer]);
          MiniGameState[playerid][AnimationTimer] = 0;
     }

     // Hide UI
     PlayerTextDrawHide(playerid, MiniGameTitle[playerid]);
     PlayerTextDrawHide(playerid, MiniGameInstructions[playerid]);
     HidePlayerProgressBar(playerid, MiniGameProgressBar[playerid]);
     SetPlayerProgressBarValue(playerid, MiniGameProgressBar[playerid], 0.0);

     // Store state before clearing
     new type = MiniGameState[playerid][Type];
     new customData = MiniGameState[playerid][CustomData];

     // Reset state
     MiniGameState[playerid][IsActive] = false;
     MiniGameState[playerid][Progress] = 0;

     // Unlock player & clear animations
     TogglePlayerControllable(playerid, true);
     ClearAnimations(playerid, true);

     // Call appropriate callback based on type
     if(success) {
          if(type == MINIGAME_TYPE_HARVEST) {
               Orange_MiniGame_OnComplete(playerid, customData);
          }
     } else {
          if(type == MINIGAME_TYPE_HARVEST) {
               Orange_MiniGame_OnFail(playerid, customData);
          }
     }

     return 1;
}

public MiniGame_OnTimeout(playerid) {
     if(!MiniGameState[playerid][IsActive]) return 0;

     Alert_Send(playerid, "~r~ERROR:~w~ Time's up!", 3000);
     MiniGame_End(playerid, false);
     return 1;
}

public MiniGame_LoopAnimation(playerid) {
     if(!MiniGameState[playerid][IsActive]) return 0;

     // Reapply harvest animation to keep it looping
     ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.0, true, false, false, false, 0, 1);
     return 1;
}

// ====== HELPER FUNCTIONS ======
stock bool:MiniGame_IsPlaying(playerid) {
     return MiniGameState[playerid][IsActive];
}

stock MiniGame_GetProgress(playerid) {
     return MiniGameState[playerid][Progress];
}
