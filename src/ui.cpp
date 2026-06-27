#include "ui.h"
#include <algorithm>

// ─────────────────────────────────────
//  BUTTON
// ─────────────────────────────────────

bool DrawButton(Rectangle rect, const char* label, bool selected)
{
    Color bgCol     = selected ? Color{162,162,162,255} : Color{130,130,130,255};
    Color highlight = {200,200,200,255};
    Color shadow    = { 80, 80, 80,255};
    Color textCol   = selected ? WHITE : Color{220,220,220,255};

    int b = 3;
    // Border
    DrawRectangle((int)rect.x,                        (int)rect.y,                         (int)rect.width,  b,            highlight); // top
    DrawRectangle((int)rect.x,                        (int)rect.y,                         b, (int)rect.height,            highlight); // left
    DrawRectangle((int)rect.x,                        (int)rect.y + (int)rect.height - b,  (int)rect.width,  b,            shadow);    // bottom
    DrawRectangle((int)rect.x + (int)rect.width - b,  (int)rect.y,                         b, (int)rect.height,            shadow);    // right
    // Fill
    DrawRectangle((int)rect.x + b, (int)rect.y + b, (int)rect.width - b*2, (int)rect.height - b*2, bgCol);

    // Label
    int fs = 22;
    int tw = MeasureText(label, fs);
    int tx = (int)rect.x + ((int)rect.width  - tw) / 2;
    int ty = (int)rect.y + ((int)rect.height - fs) / 2;
    DrawText(label, tx+2, ty+2, fs, Color{40,40,40,200}); // shadow
    DrawText(label, tx,   ty,   fs, textCol);

    // Hover tint
    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    if (hovered)
        DrawRectangle((int)rect.x+b, (int)rect.y+b, (int)rect.width-b*2, (int)rect.height-b*2, Color{255,255,255,30});

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// ─────────────────────────────────────
//  PAUSE MENU
// ─────────────────────────────────────

int DrawPauseMenu(int& opt)
{
    int result = 0;

    // Keyboard nav
    if (IsKeyPressed(KEY_DOWN)) opt++;
    if (IsKeyPressed(KEY_UP))   opt--;
    opt = std::clamp(opt, 0, 2);

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int btnW = 320, btnH = 44;
    int cx = sw/2 - btnW/2;

    // Dim background
    DrawRectangle(0, 0, sw, sh, Color{0,0,0,140});

    // Title
    const char* title = "Game Menu";
    int tSize = 48;
    int tW    = MeasureText(title, tSize);
    int tY    = sh/2 - 190;
    DrawText(title, sw/2 - tW/2 + 3, tY+3, tSize, Color{40,40,40,200});
    DrawText(title, sw/2 - tW/2,     tY,   tSize, WHITE);

    // ── Back to Game ──
    Rectangle rResume   = { (float)cx, (float)(tY + tSize + 24),  (float)btnW, (float)btnH };
    // ── Settings ──
    Rectangle rSettings = { (float)cx, (float)(sh/2 - btnH/2),    (float)btnW, (float)btnH };
    // ── Quit ──
    Rectangle rQuit     = { (float)cx, (float)(sh - 80),           (float)btnW, (float)btnH };

    // Mouse hover syncs keyboard selection
    Vector2 mouse = GetMousePosition();
    if (CheckCollisionPointRec(mouse, rResume))   opt = 0;
    if (CheckCollisionPointRec(mouse, rSettings)) opt = 1;
    if (CheckCollisionPointRec(mouse, rQuit))     opt = 2;

    bool enter = IsKeyPressed(KEY_ENTER);

    if (DrawButton(rResume,   "Back to Game", opt == 0) || (opt == 0 && enter)) result = 1;
    if (DrawButton(rSettings, "Settings",     opt == 1) || (opt == 1 && enter)) result = 2;
    if (DrawButton(rQuit,     "Quit Game",    opt == 2) || (opt == 2 && enter)) result = 3;

    return result;
}
