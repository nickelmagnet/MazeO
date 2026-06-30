#pragma once
#include "raylib.h"


enum ThemeId {
    THEME_DEFAULT = 0,
    THEME_WINTER,
    THEME_NETHER,
    THEME_SPACE,
    THEME_BEACH,
    THEME_COUNT
};

struct Theme {
    const char* name;
    const char* wallTexPath;
    const char* floorTexPath;
    const char* orbTexPath;
	const char* musicPath;
    Color sky;
	float fogDensity;
};
//extern const Theme THEMES[THEME_Ct/OUNT];
const Theme THEMES[THEME_COUNT] = {
   { "Default", "assets/themes/default/wall.png", "assets/themes/default/floor.png", "assets/themes/default/orb.png", "assets/themes/default/music.ogg", SKYBLUE,        0.02f  },
   { "Winter",  "assets/themes/winter/wall.png",  "assets/themes/winter/floor.png",  "assets/themes/winter/orb.png",  "assets/themes/winter/music.ogg",  WHITE,          0.05f },
   { "Nether",  "assets/themes/nether/wall.png",  "assets/themes/nether/floor.png",  "assets/themes/nether/orb.png",  "assets/themes/nether/music.ogg",  BLACK,          0.06f },
   { "Space",   "assets/themes/space/wall.png",   "assets/themes/space/floor.png",   "assets/themes/space/orb.png",   "assets/themes/space/music.ogg",   DARKBLUE,       0.0f  },
   { "Summer",  "assets/themes/summer/wall.png",  "assets/themes/summer/floor.png",  "assets/themes/summer/orb.png",  "assets/themes/summer/music.ogg",  {26,0,255,255}, 0.0f  }
};
struct GameSettings {
    float sensitivity = 0.003f;
    float fov = 60.0f;
    float  MaxFps = 60;
    int themeId = THEME_DEFAULT;
	float musicVolume = 0.5f;
};

static const char* MAIN_MENU_MUSIC = "assets/main/menu_music.ogg";
static const char* VICTORY_MUSIC = "assets/main/victory_music.ogg";
static const char* MAIN_MENU_TEXTURE = "assets/main/menu_bg.png";
static const char* VICTORY_BG_TEXTURE = "assets/main/victory_bg.png";

bool DrawButton(Rectangle rect, const char* label, bool selected, Color bgOverride = { 162,162,162,255 }, bool noHover = false); //generates button & returns true if clicked

// returns 1=play, 2=quit
int DrawMainMenu();

// returns 1=back to game, 2=settings, 3=quit
int DrawPauseMenu(int& opt);

// returns 1=play again, 2=main menu, 3=quit
int DrawLevelComplete();

int DrawDifficultyMenu(); // returns 1-5 for difficulty, 0 for nothing

int DrawSettingsMenu(GameSettings& s);