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

bool DrawButton(Rectangle rect, const char* label, bool selected, Color bgOverride,bool noHover)
{
    Color bgCol   = bgOverride;
    Color textCol = WHITE;
    int b = 3;

    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, rect);

    Color borderCol = (!noHover && hovered) || selected ? WHITE : BLACK;

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

static float DrawSlider(Rectangle rail, float value, float minVal, float maxVal, const char* label, bool isSelected, int displayMode)
{
    // displayMode: 0 = normal scalar, 1 = percentage, 2 = FPS custom display
    const char* valStr = "";
    if (displayMode == 1) {
        valStr = TextFormat("%s: %.0f%%", label, value);
    }
    else if (displayMode == 2) {
        valStr = (value >= maxVal - 0.5f) ? TextFormat("%s: Unlimited", label) : TextFormat("%s: %.0f", label, value);
    }
    else {
        valStr = TextFormat("%s: %.0f", label, value);
    }

    int tw = MeasureText(valStr, 20);
    DrawText(valStr, (int)(rail.x + rail.width / 2 - tw / 2), (int)rail.y - 26, 20, WHITE);

    DrawButton(rail, "", false, Color{ 60,60,60,255 }, true);

    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, rail)) {
        float t = (mouse.x - rail.x) / rail.width;
        t = std::clamp(t, 0.0f, 1.0f);
        value = minVal + t * (maxVal - minVal);
    }

    float t = (value - minVal) / (maxVal - minVal);
    int knobW = 30;
    Rectangle knob = { rail.x + t * (rail.width - knobW), rail.y, (float)knobW, rail.height };

    bool knobHovered = CheckCollisionPointRec(mouse, knob);
    DrawButton(knob, "", knobHovered || isSelected, Color{ 130,130,130,255 }, false);

    return value;
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
    DrawRectangle(0, 0, sw, sh, Color{ 0,0,0,140 });

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
    Rectangle rPlay = { (float)cx, (float)btnStartY,                        (float)BTN_W, (float)BTN_H };
    Rectangle rSettings = { (float)cx, (float)(btnStartY+ BTN_H + BTN_GAP),  (float)BTN_W, (float)BTN_H };
    Rectangle rQuit = { (float)cx, (float)(btnStartY + 2*(BTN_H + BTN_GAP)),(float)BTN_W, (float)BTN_H };

    // Keyboard nav
    if (IsKeyPressed(KEY_DOWN)) opt++;
    if (IsKeyPressed(KEY_UP))   opt--;
    opt = std::clamp(opt, 0, 2);

    bool enter = IsKeyPressed(KEY_ENTER);
    if (DrawButton(rPlay, "Play Game", opt == 0) || (opt == 0 && enter)) result = 1;
    if (DrawButton(rSettings, "Settings", opt == 1) || (opt == 1 && enter)) result = 2;
    if (DrawButton(rQuit, "Quit Game", opt == 2) || (opt == 2 && enter)) result = 3;

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
    DrawRectangle(0, 0, sw, sh, Color{ 0,0,0,140 });

    // Title
    int titleY = sh / 2 - 160;
    DrawTitle("You Escaped!", 56, titleY);

    // Subtitle
    const char* sub = "Nice Job!";
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

    DrawRectangle(0, 0, sw, sh, Color{0,0,0,140 });

    int titleY = sh / 2 - 230;
    DrawTitle("Select Difficulty", 48, titleY);

    const char* labels[] = { "Bot", "Noob", "Pro", "Hacker", "God" };
    const char* sizes[] = { "9x9", "15x15", "21x21", "27x27", "33x33" };
    const Color colors[] = { BLUE, GREEN, {200,200,200,255}, DARKGRAY, RED };

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

        if (DrawButton(r, label, opt == i,colors[i]) || (opt == i && enter)) result = i + 1;
    }

    // Back button
    Rectangle rBack = { (float)cx, (float)(sh - 80), (float)BTN_W, (float)BTN_H };
    if (DrawButton(rBack, "Back", opt == totalBtns) || (opt == totalBtns && enter)) result = -1;

    return result; // 1-5 = difficulty chosen, -1 = back
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

    DrawRectangle(0, 0, sw, sh, Color{ 0,0,0,140});

    int titleY = sh / 2 - 260;
    DrawTitle("Options", 48, titleY);

    float sliderW = BTN_W;
    float sliderH = BTN_H;
    float col1X = sw / 2 - BTN_W - 12;
    float col2X = sw / 2 + 12;
    float row1Y = titleY + 48 + 48;
    float row2Y = row1Y + sliderH + 56;
    float row3Y = row2Y + sliderH + 56;

    static int opt = 0; 

    // ── Keyboard Navigation ──
    if (opt == 0) {
        if (IsKeyPressed(KEY_RIGHT))  s.fov += 1.0f;
        if (IsKeyPressed(KEY_LEFT))  s.fov -= 1.0f;
        s.fov = std::clamp(s.fov, 40.0f, 120.0f);
    }
    else if (opt == 1) {
        if (IsKeyPressed(KEY_RIGHT)) s.sensitivity += 0.000045f;
        if (IsKeyPressed(KEY_LEFT))  s.sensitivity -= 0.000045f;
        s.sensitivity = std::clamp(s.sensitivity, 0.001f, 0.010f);
    }
    else if (opt == 2) {
        if (IsKeyPressed(KEY_RIGHT)) s.MaxFps += 1.0f;
        if (IsKeyPressed(KEY_LEFT))  s.MaxFps -= 1.0f;
        s.MaxFps = std::clamp(s.MaxFps, 30.0f, 241.0f);
    }
    else if (opt == 3) {
        if (IsKeyPressed(KEY_RIGHT)) s.themeId = (s.themeId + 1) % THEME_COUNT;
        if (IsKeyPressed(KEY_LEFT))  s.themeId = (s.themeId - 1 + THEME_COUNT) % THEME_COUNT;
    }
    else if (opt == 4) {
        if (IsKeyPressed(KEY_RIGHT)) s.musicVolume += 0.05f;
        if (IsKeyPressed(KEY_LEFT))  s.musicVolume -= 0.05f;
        s.musicVolume = std::clamp(s.musicVolume, 0.0f, 1.0f);
    }

    // ── Draw Row 1
    Rectangle rFOV = { col1X, row1Y, sliderW, sliderH };
    s.fov = DrawSlider(rFOV, s.fov, 40.0f, 120.0f, "FOV", opt == 0, false);

    Rectangle rSens = { col2X, row1Y, sliderW, sliderH }; // Fixed Row alignment layout bug here
    float visualSens = 1.0f + ((s.sensitivity - 0.001f) / (0.010f - 0.001f)) * 199.0f;
    visualSens = DrawSlider(rSens, visualSens, 1.0f, 200.0f, "Sensitivity", opt == 1, 1);
    s.sensitivity = 0.001f + ((visualSens - 1.0f) / 199.0f) * (0.010f - 0.001f);

    
    Rectangle rFPS = { col1X, row2Y, sliderW, sliderH };
    s.MaxFps = DrawSlider(rFPS, s.MaxFps, 30.0f, 241.0f, "Max FPS", opt == 2, 2);
    Rectangle rTheme = { col2X, row2Y, sliderW, sliderH };
    const char* themeLabel = TextFormat("Theme: %s", THEMES[s.themeId].name);
    if (DrawButton(rTheme, themeLabel, opt == 3)) {
        s.themeId = (s.themeId + 1) % THEME_COUNT;
    }

    Rectangle rVolume = { col1X, row3Y, sliderW, sliderH };
    float visualVol = s.musicVolume * 100.0f;
    visualVol = DrawSlider(rVolume, visualVol, 0.0f, 100.0f, "Music Volume", opt == 4, 1);
    s.musicVolume = visualVol / 100.0f;

    // ── Up / Down UI focus Selection navigation loop ──
    if (IsKeyPressed(KEY_DOWN)) opt++;
    if (IsKeyPressed(KEY_UP))   opt--;
    opt = std::clamp(opt, 0, 5);

    bool enter = IsKeyPressed(KEY_ENTER);
    Rectangle rDone = { (float)cx, (float)(sh - 80), (float)BTN_W, (float)BTN_H };

    if (DrawButton(rDone, "Done", opt == 5) || (opt == 5 && enter) || IsKeyPressed(KEY_ESCAPE)) {
        result = -1;
    }

    return result;
}