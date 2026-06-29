#pragma once
#include "raylib.h"

struct GameSettings {
    float sensitivity = 0.003f;
    float fov = 60.0f;
    float  MaxFps = 0;   
};

bool DrawButton(Rectangle rect, const char* label, bool selected, Color bgOverride = { 162,162,162,255 }, bool noHover = false); //generates button & returns true if clicked

// returns 1=play, 2=quit
int DrawMainMenu();

// returns 1=back to game, 2=settings, 3=quit
int DrawPauseMenu(int& opt);

// returns 1=play again, 2=main menu, 3=quit
int DrawLevelComplete();

int DrawDifficultyMenu(); // returns 1-5 for difficulty, 0 for nothing

int DrawSettingsMenu(GameSettings& s);