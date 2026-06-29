#pragma once
#include "raylib.h"

bool DrawButton(Rectangle rect, const char* label, bool selected); //generates button & returns true if clicked

// returns 1=play, 2=quit
int DrawMainMenu();

// returns 1=back to game, 2=settings, 3=quit
int DrawPauseMenu(int& opt);

// returns 1=play again, 2=main menu, 3=quit
int DrawLevelComplete();

int DrawDifficultyMenu(); // returns 1-5 for difficulty, 0 for nothing