#pragma once
#include "raylib.h"

// ─────────────────────────────────────
//  UI
// ─────────────────────────────────────

// MC-style beveled button. Returns true if clicked this frame.
// Pass selected=true to highlight it (keyboard nav or mouse hover).
bool DrawButton(Rectangle rect, const char* label, bool selected);

// Draws the full pause menu overlay.
// opt = currently selected button index (0=resume, 1=settings, 2=quit)
// Returns: 0 = nothing, 1 = resume, 2 = settings, 3 = quit
int DrawPauseMenu(int& opt);
