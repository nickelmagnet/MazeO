#include "ui.h"
#include <algorithm>

// ─────────────────────────────────────
//  BUTTON
// ─────────────────────────────────────

static const int BTN_W = 320;
static const int BTN_H = 44;
static const int BTN_GAP = 12;

static int CenterX() { return GetScreenWidth() / 2 - BTN_W / 2; }

static int DrawTitle(const char* text, int fontSize, int y)
{
    int sw = GetScreenWidth();
    int tw = MeasureText(text, fontSize);
    DrawText(text, sw / 2 - tw / 2 + 3, y + 3, fontSize, Color{ 40,40,40,200 });
    DrawText(text, sw / 2 - tw / 2, y, fontSize, WHITE);
    return y + fontSize;
}

bool DrawButton(Rectangle rect, const char* label, bool selected)
{
    Color bgCol     = {162,162,162,255};
    Color textCol = WHITE;
    int b = 3;

    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);
    Color borderCol = hovered||selected ? WHITE : BLACK;
    DrawRectangleLinesEx(rect, 2, borderCol);
    // Border
    DrawRectangle(rect.x+2,rect.y+2, rect.width-4, b, { 200,200,200,255 }); // top
    DrawRectangle(rect.x+2,rect.y+2,  b,rect.height-4, { 200,200,200,255 }); // left
    DrawRectangle(rect.x+2,rect.y+rect.height - 2*b-4,rect.width-4,  2*b, { 80, 80, 80,255 }); // bottom
    DrawRectangle(rect.x+ rect.width - b-4,rect.y+2, b,rect.height-4, { 80, 80, 80,255 });  // right
    // Fill
    DrawRectangle(rect.x +2+ b,rect.y +2+ b,rect.width - 2*b-4,rect.height - b*3-4, bgCol);

    // Label
    int fs = 22.0f;
    int tw = MeasureText(label, fs);
    int tx =  rect.x + ( rect.width  - tw) / 2;
    int ty =  rect.y + ( rect.height - fs) / 2;
    DrawText(label, tx+2, ty+2, fs, Color{40,40,40,200}); // shadow
    DrawText(label, tx,   ty,   fs, textCol);

    return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}


// ─────────────────────────────────────
//  MAIN MENU
// ─────────────────────────────────────

int DrawMainMenu()
{
    static int opt = 0;
    int result = 0;

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int cx = CenterX();

    // Dark background
    DrawRectangle(0, 0, sw, sh, Color{ 20,20,20,255 });

    // Title — big, two lines like MC
    int titleY = sh / 2 - 210;
    DrawTitle("MazeO", 72, titleY);

    // Subtitle
    const char* sub = "Can you find the way out?";
    int subW = MeasureText(sub, 18);
    DrawText(sub, sw / 2 - subW / 2 + 2, titleY + 72 + 12 + 2, 18, Color{ 40,40,40,200 });
    DrawText(sub, sw / 2 - subW / 2, titleY + 72 + 12, 18, Color{ 180,180,180,255 });

    // Buttons
    int btnStartY = sh / 2 - BTN_H / 2 + 20;
    Rectangle rPlay = { (float)cx, (float)btnStartY,                       (float)BTN_W, (float)BTN_H };
    Rectangle rQuit = { (float)cx, (float)(btnStartY + BTN_H + BTN_GAP),   (float)BTN_W, (float)BTN_H };

    // Keyboard nav
    if (IsKeyPressed(KEY_DOWN)) opt++;
    if (IsKeyPressed(KEY_UP))   opt--;
    opt = std::clamp(opt, 0, 1);

    bool enter = IsKeyPressed(KEY_ENTER);
    if (DrawButton(rPlay, "Play Game", opt == 0) || (opt == 0 && enter)) result = 1;
    if (DrawButton(rQuit, "Quit Game", opt == 1) || (opt == 1 && enter)) result = 2;

    return result;
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
    int cx = CenterX();

    // Dim background
    DrawRectangle(0, 0, sw, sh, Color{0,0,0,140});

    // Title
    const char* title = "Game Menu";
    int fontsize = 48;
    int tW    = MeasureText(title, fontsize);
    int tY    = sh/2 - 190;

    DrawTitle("Game Menu", 48, tY);
    int btnStartY = tY + 48 + 24;
    // ── Back to Game ──
    Rectangle rResume   = { (float)cx, (float)(tY+fontsize+24), (float)BTN_W, (float)BTN_H };
    // ── Settings ──
    Rectangle rSettings = { (float)cx, (float)(sh/2-BTN_H/2),(float)BTN_W, (float)BTN_H };
    // ── Quit ──
    Rectangle rQuit     = { (float)cx, (float)(sh-80),  (float)BTN_W, (float)BTN_H };


    bool enter = IsKeyPressed(KEY_ENTER);

    if (DrawButton(rResume,   "Back to Game", opt == 0) || (opt == 0 && enter)) result = 1;
    if (DrawButton(rSettings, "Settings",     opt == 1) || (opt == 1 && enter)) result = 2;
    if (DrawButton(rQuit,     "Quit to menu",    opt == 2) || (opt == 2 && enter)) result = 3;

    return result;
}

// ─────────────────────────────────────
//  LEVEL COMPLETE
// ─────────────────────────────────────

int DrawLevelComplete()
{
    static int opt = 0;
    int result = 0;

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int cx = CenterX();

    // Dark overlay
    DrawRectangle(0, 0, sw, sh, Color{ 0,0,0,180 });

    // Title
    int titleY = sh / 2 - 160;
    DrawTitle("You Escaped!", 56, titleY);

    // Subtitle
    const char* sub = "Pure AURA!";
    int subW = MeasureText(sub, 18);
    DrawText(sub, sw / 2 - subW / 2 + 2, titleY + 56 + 12 + 2, 18, Color{ 40,40,40,200 });
    DrawText(sub, sw / 2 - subW / 2, titleY + 56 + 12, 18, Color{ 180,180,180,255 });

    // Buttons
    int btnStartY = sh / 2 + 10;
    Rectangle rAgain = {(float)cx, (float)btnStartY,                        (float)BTN_W, (float)BTN_H };
    Rectangle rLevel = {(float)cx, (float)btnStartY + BTN_H + BTN_GAP,      (float)BTN_W, (float)BTN_H };
    Rectangle rMenu  = {(float)cx, (float)(btnStartY +2*( BTN_H + BTN_GAP)),(float)BTN_W, (float)BTN_H };
    Rectangle rQuit  = {(float)cx, (float)(sh - 80),                        (float)BTN_W, (float)BTN_H };

    // Keyboard nav
    if (IsKeyPressed(KEY_DOWN)) opt++;
    if (IsKeyPressed(KEY_UP))   opt--;
    opt = std::clamp(opt, 0, 3);


    bool enter = IsKeyPressed(KEY_ENTER);
    if (DrawButton(rAgain, "Play Again", opt == 0) || (opt == 0 && enter)) result = 1;
    if (DrawButton(rLevel, "Change Difficulty", opt == 1) || (opt == 1 && enter)) result = 2;
    if (DrawButton(rMenu, "Main Menu", opt == 2) || (opt == 2 && enter)) result = 3;
    if (DrawButton(rQuit, "Quit Game", opt == 3) || (opt == 3 && enter)) result = 4;

    return result;
}


// ─────────────────────────────────────
// DIFFICULTY MENU
// ─────────────────────────────────────

int DrawDifficultyMenu()
{
    static int opt = 0;
    int result = 0;

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int cx = CenterX();

    DrawRectangle(0, 0, sw, sh, Color{ 20,20,20,255 });

    int titleY = sh / 2 - 230;
    DrawTitle("Select Difficulty", 48, titleY);

    const char* labels[] = { "Bot", "Noob", "Pro", "Hacker", "God" };
    const char* sizes[] = { "9x9", "15x15", "21x21", "27x27", "33x33" };

    int totalBtns = 5;
    int btnStartY = titleY + 48 + 30;

    // Keyboard nav
    if (IsKeyPressed(KEY_DOWN)) opt++;
    if (IsKeyPressed(KEY_UP))   opt--;
    opt = std::clamp(opt, 0, totalBtns);

    bool enter = IsKeyPressed(KEY_ENTER);

    for (int i = 0; i < totalBtns; i++) {
        Rectangle r = {
            (float)cx,(float)(btnStartY + i * (BTN_H + BTN_GAP)),(float)BTN_W,(float)BTN_H};

        // label like "Noob (9x9)"
        const char* label = TextFormat("%-10s  (%s)", labels[i], sizes[i]);

        if (DrawButton(r, label, opt == i) || (opt == i && enter)) result = i + 1;
    }

    // Back button
    Rectangle rBack = { (float)cx, (float)(sh - 80), (float)BTN_W, (float)BTN_H };
    if (DrawButton(rBack, "Back", opt == totalBtns) || (opt == totalBtns && enter)) result = -1;

    return result; // 1-5 = difficulty chosen, -1 = back
}

static float DrawSlider(Rectangle rail, float value, float minVal, float maxVal, const char* label)
{
    int sw = GetScreenWidth();

    // Label left, value right
    const char* valStr = TextFormat("%s: %.0f", label, value);
    DrawText(valStr, (int)rail.x, (int)rail.y - 22, 20, WHITE);

    // Rail background
    DrawRectangleRec(rail, Color{ 60,60,60,255 });
    DrawRectangleLinesEx(rail, 2, Color{ 80,80,80,255 });

    // Fill
    float t = (value - minVal) / (maxVal - minVal);
    DrawRectangle((int)rail.x, (int)rail.y, (int)(rail.width * t), (int)rail.height, Color{ 162,162,162,255 });

    // Knob
    int knobX = (int)(rail.x + rail.width * t);
    int knobY = (int)(rail.y + rail.height / 2);
    DrawCircle(knobX, knobY, 8, WHITE);
    DrawCircleLines(knobX, knobY, 8, BLACK);

    // Drag
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, rail)) {
        t = (mouse.x - rail.x) / rail.width;
        t = t < 0 ? 0 : (t > 1 ? 1 : t);
        value = minVal + t * (maxVal - minVal);
    }

    return value;
}

// ─────────────────────────────────────
//  SETTINGS MENU
//  returns -1 = back/done
// ─────────────────────────────────────

int DrawSettingsMenu(GameSettings& s)
{
    int result = 0;

    int sw = GetScreenWidth();
    int sh = GetScreenHeight();
    int cx = CenterX();

    DrawRectangle(0, 0, sw, sh, Color{ 20,20,20,255 });

    // Title
    int titleY = sh / 2 - 260;
    DrawTitle("Options", 48, titleY);

    int sliderW = BTN_W;
    int sliderH = 16;
    int col1X = sw / 2 - BTN_W - 8;   // left column
    int col2X = sw / 2 + 8;            // right column
    int row1Y = titleY + 48 + 40;

    // ── Row 1: FOV slider | Sensitivity slider ──
    Rectangle rFOV = { (float)col1X, (float)(row1Y + 22), (float)sliderW, (float)sliderH };
    Rectangle rSens = { (float)col2X, (float)(row1Y + 22), (float)sliderW, (float)sliderH };

    s.fov = DrawSlider(rFOV, s.fov, 40.0f, 120.0f, "FOV");
    s.sensitivity = DrawSlider(rSens, s.sensitivity * 1000.0f, 1.0f, 10.0f, "Sensitivity") / 1000.0f;

    // ── Row 2: FPS toggle button ──
    int row2Y = row1Y + sliderH + 60;
    Rectangle rFPS = { (float)col1X, (float)row2Y, (float)sliderW, (float)BTN_H };

    const char* fpsLabel = TextFormat("FPS Cap: %s", FPS_LABELS[s.fpsCapIndex]);
    if (DrawButton(rFPS, fpsLabel, false)) {
        s.fpsCapIndex = (s.fpsCapIndex + 1) % FPS_COUNT; // cycle through options
    }

    // ── Done button ──
    Rectangle rDone = { (float)cx, (float)(sh - 80), (float)BTN_W, (float)BTN_H };
    if (DrawButton(rDone, "Done", false) || IsKeyPressed(KEY_ESCAPE)) result = -1;

    return result;
}