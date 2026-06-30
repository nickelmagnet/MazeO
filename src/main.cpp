#include "raylib.h"
#include "maze.h"
#include "player.h"
#include "ui.h"

// ─────────────────────────────────────
//  GAME STATES
// ─────────────────────────────────────
enum class GameState {
    MainMenu,
    Playing,
    Paused,
    LevelComplete,
    Settings,
    DifficultyMenu,
};

int main()
{
    InitWindow(1280, 760, "MazeO");
    InitAudioDevice();
    SetTargetFPS(0);
    SetExitKey(KEY_NULL);

	GameSettings settings;

    // ── Maze + player (initialized on Play) ──
    Maze   maze = {};
    Player player = {};
    Vector3 exitPos = {};
    bool gameStarted = false;

    Texture2D wallTex[THEME_COUNT];
    Texture2D floorTex[THEME_COUNT];
    Texture2D orbTex[THEME_COUNT];
    Music     themeMusic[THEME_COUNT];
    for (int i = 0; i < THEME_COUNT; i++) {
        wallTex[i] = LoadTexture(THEMES[i].wallTexPath);
        floorTex[i] = LoadTexture(THEMES[i].floorTexPath);
        orbTex[i] = LoadTexture(THEMES[i].orbTexPath);
        themeMusic[i] = LoadMusicStream(THEMES[i].musicPath);
        themeMusic[i].looping = true;
    }

    const float CELL = 3.0f;
    const float WALL_THICK = 0.3f;
    const float WALL_H = 3.0f;

    Model wallModelNS = LoadModelFromMesh(GenMeshCube(CELL, WALL_H, WALL_THICK));
    Model wallModelEW = LoadModelFromMesh(GenMeshCube(WALL_THICK, WALL_H, CELL));
    Model floorTileModel = LoadModelFromMesh(GenMeshCube(CELL, 0.1f, CELL));
    Model orbModel = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

    Music mainMenuMusic = LoadMusicStream(MAIN_MENU_MUSIC);
    Music victoryMusic = LoadMusicStream(VICTORY_MUSIC);
    mainMenuMusic.looping = true;
    victoryMusic.looping = false;
    Texture2D MainBg = LoadTexture(MAIN_MENU_TEXTURE);

    const int difficulty[][2] = { {9,9},{15,15},{21,21},{27,27},{33,33} };
    int currentLevel = 0;

     
    auto StartNewGame = [&](int level) {
        int rows = difficulty[level][0];
        int cols = difficulty[level][1];
        maze = GenerateMaze(rows, cols, CELL, WALL_THICK, WALL_H);

        // random spawn — any cell
        int spawnR = GetRandomValue(0, maze.rows - 1);
        int spawnC = GetRandomValue(0, maze.cols - 1);
        player = CreatePlayer(spawnR, spawnC, maze);

        // exit — opposite corner from spawn
        int exitR = (spawnR < maze.rows / 2) ? GetRandomValue(maze.rows / 2, maze.rows - 1)
            : GetRandomValue(0, maze.rows / 2);
        int exitC = (spawnC < maze.cols / 2) ? GetRandomValue(maze.cols / 2, maze.cols - 1)
            : GetRandomValue(0, maze.cols / 2);

        // exit world position
        float ox = -maze.width * 0.5f;
        float oz = -maze.depth * 0.5f;
        exitPos = { ox + (exitC + 0.5f) * maze.cellSize, 0.5f, oz + (exitR + 0.5f) * maze.cellSize };
        };

	// ── Music ──
    int currentMusicTrack = -1; 
    auto PlayTrack = [&](int track) {
        if (currentMusicTrack == track) return; // already playing this one
        // stop whatever was playing
        if (currentMusicTrack == 5)      StopMusicStream(mainMenuMusic);
        else if (currentMusicTrack == 6) StopMusicStream(victoryMusic);
        else if (currentMusicTrack >= 0) StopMusicStream(themeMusic[currentMusicTrack]);

        currentMusicTrack = track;
        if (track == 5) { PlayMusicStream(mainMenuMusic); SetMusicVolume(mainMenuMusic, settings.musicVolume); }
        else if (track == 6) { PlayMusicStream(victoryMusic);  SetMusicVolume(victoryMusic, settings.musicVolume); }
        else if (track >= 0) { PlayMusicStream(themeMusic[track]); SetMusicVolume(themeMusic[track], settings.musicVolume); }
        };

	// -─ Drawing World ──
    auto DrawGameWorld3D = [&]() {
        const Theme& activeTheme = THEMES[settings.themeId];
        player.camera.fovy = settings.fov;
        gameStarted = true;
        wallModelNS.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wallTex[settings.themeId];
        wallModelEW.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = wallTex[settings.themeId];

        BeginMode3D(player.camera);
        DrawCube({ 0, 20.0f, 0 }, 2000.0f, 0.1f, 2000.0f, activeTheme.sky);

        floorTileModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = floorTex[settings.themeId];
        float ox = -maze.width * 0.5f;
        float oz = -maze.depth * 0.5f;

        for (int r = 0; r < maze.rows; r++) {
            for (int c = 0; c < maze.cols; c++) {
                Vector3 tilePos = { ox + (c + 0.5f) * CELL, -0.05f, oz + (r + 0.5f) * CELL };

                Color tint = WHITE;
                if (activeTheme.fogDensity > 0.0f) {
                    float dx = tilePos.x - player.camera.position.x;
                    float dz = tilePos.z - player.camera.position.z;
                    float dist = sqrtf(dx * dx + dz * dz);
                    float fogAmt = std::min(1.0f, dist * activeTheme.fogDensity);
                    tint = ColorLerp(WHITE, activeTheme.sky, fogAmt);
                }

                DrawModelEx(floorTileModel, tilePos, { 0, 1, 0 }, 0.0f, { 1.0f, 1.0f, 1.0f }, tint);
            }
        }

        for (auto& w : maze.walls) {
            bool isNS = w.size.x > w.size.z;
            Model& m = isNS ? wallModelNS : wallModelEW;
            Color tint = WHITE;
            if (activeTheme.fogDensity > 0.0f) {
                float dx = w.center.x - player.camera.position.x;
                float dz = w.center.z - player.camera.position.z;
                float dist = sqrtf(dx * dx + dz * dz);
                float fogAmt = std::min(1.0f, dist * activeTheme.fogDensity);
                tint = ColorLerp(WHITE, activeTheme.sky, fogAmt);
            }
            DrawModelEx(m, w.center, { 0,1,0 }, 0.0f, { 1,1,1 }, tint);
        }

        DrawSphere(exitPos, 0.4f, { 0, 0, 0, 0 });
        orbModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = orbTex[settings.themeId];
        float rotationTime = (float)GetTime() * 90.0f;
        float bounceOffset = sinf((float)GetTime() * 3.0f) * 0.15f;
        Vector3 floatingPos = { exitPos.x, exitPos.y + bounceOffset + 0.2f, exitPos.z };
        Vector3 gemScale = { 0.5f, 0.5f, 0.5f };
        DrawModelEx(orbModel, floatingPos, { 1, 1, 0 }, rotationTime, gemScale, WHITE);

        EndMode3D();

        DrawText("WASD: move  |  Mouse: look  |  Shift: sprint  |  Esc: pause", 10, 10, 18, BLACK);
        };

    auto DrawMainMenuBg = [&]() {
        DrawTexturePro(MainBg,
            { 0,0,(float)MainBg.width,(float)MainBg.height },
            { 0,0,(float)GetScreenWidth(),(float)GetScreenHeight() },
            { 0,0 }, 0.0f, WHITE);
        };
        // ── State ──
    GameState state = GameState::MainMenu;
    EnableCursor();
    int pauseOpt = 0;
    GameState prevState = GameState::MainMenu;
	PlayTrack(5); // main menu music
	

    //  GAME LOOP
    while (!WindowShouldClose())
    {   
        BeginDrawing();
        ClearBackground(SKYBLUE);
        float dt = GetFrameTime();

        if (settings.MaxFps >= 241.0f) {SetTargetFPS(0);}
        else {SetTargetFPS((int)settings.MaxFps);}

        if (currentMusicTrack == 5)      UpdateMusicStream(mainMenuMusic);
        else if (currentMusicTrack == 6) UpdateMusicStream(victoryMusic);
        else if (currentMusicTrack >= 0) UpdateMusicStream(themeMusic[currentMusicTrack]);

        // ── Escape toggles pause ──
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (state == GameState::Playing) {
                state = GameState::Paused;
                EnableCursor();
            } else if (state == GameState::Paused) {
                state = GameState::Playing;
                DisableCursor();
            }
            else if (state == GameState::Settings) {
                state = prevState; }
            else if (state == GameState::DifficultyMenu) { state = prevState; }
            else if (state == GameState::MainMenu) { break; }
        }

        // ── Update ──
        if (state == GameState::Playing) {
            DrawGameWorld3D();
            UpdatePlayer(player, maze.walls,dt,settings.sensitivity);

            float dx = player.camera.position.x - exitPos.x;
            float dz = player.camera.position.z - exitPos.z;
            float dist = sqrtf(dx * dx + dz * dz);
            if (dist < 0.8f) {
                state = GameState::LevelComplete;
                EnableCursor();
                PlayTrack(6);
            }
        }

        if (state == GameState::MainMenu) {
            DrawMainMenuBg();
            int action = DrawMainMenu();
            if (action == 1) {          // Choose Difficulty
                prevState = state;
                state = GameState::DifficultyMenu;
			}
			else if (action == 2) {     // Settings
				prevState = state;
				state = GameState::Settings;
			}
			else if (action == 3) {  // Exit
				break;
            }
        }else if (state == GameState::DifficultyMenu) {
            if (prevState == GameState::LevelComplete) { DrawGameWorld3D();}
            else {DrawMainMenuBg();}

            int action = DrawDifficultyMenu();
            if (action > 0) {
                currentLevel = action - 1;
                StartNewGame(currentLevel);
                state = GameState::Playing;
                DisableCursor();
                PlayTrack(settings.themeId); // transition: entering gameplay
            }
            else if (action == -1) {
                state = prevState;
            }
        }
        else if (state == GameState::Paused) {
            DrawGameWorld3D();
                int action = DrawPauseMenu(pauseOpt);
                if (action == 1) { state = GameState::Playing; DisableCursor(); }
                else if (action == 2) { prevState = GameState::Paused; state = GameState::Settings; }
                else if (action == 3) {
                    state = GameState::MainMenu;
                    EnableCursor();
                    PlayTrack(5); // transition: leaving game -> main menu music
            }
        } else if (state == GameState::Settings) {
            if (gameStarted && prevState == GameState::Paused) {DrawGameWorld3D();}
            else {DrawMainMenuBg();}

            int oldTheme = settings.themeId;
            int action = DrawSettingsMenu(settings);

            if (currentMusicTrack == 5)      SetMusicVolume(mainMenuMusic, settings.musicVolume);
            else if (currentMusicTrack == 6) SetMusicVolume(victoryMusic, settings.musicVolume);
            else if (currentMusicTrack >= 0) SetMusicVolume(themeMusic[currentMusicTrack], settings.musicVolume);

            if (settings.themeId != oldTheme && currentMusicTrack >= 0 && currentMusicTrack < THEME_COUNT) {
                PlayTrack(settings.themeId);
            }

            if (action == -1) {
                state = prevState;
                int frm = (settings.MaxFps >= 241.0f) ? 0 : (int)settings.MaxFps;
                SetTargetFPS(frm);
                if (prevState == GameState::Paused) EnableCursor();
            }
        }else if (state == GameState::LevelComplete) {
            DrawGameWorld3D();

            int action = DrawLevelComplete();
            if (action == 1) {          // Play again
                StartNewGame(currentLevel);
                state = GameState::Playing;
                DisableCursor();
                PlayTrack(settings.themeId); // transition: victory -> gameplay
            }
            else if (action == 2) { prevState = state; state = GameState::DifficultyMenu; }
            else if (action == 3) {
                state = GameState::MainMenu;PlayTrack(5);
            }
            else if (action == 4) {   // Quit
                break;
            }
        }

        EndDrawing();
    }

    for (int i = 0; i < THEME_COUNT; i++) {
        UnloadTexture(wallTex[i]);
        UnloadTexture(floorTex[i]);
		UnloadTexture(orbTex[i]);
        UnloadMusicStream(themeMusic[i]);
    }
    UnloadModel(wallModelNS);
    UnloadModel(wallModelEW);
    UnloadModel(floorTileModel);
	UnloadModel(orbModel);
    UnloadMusicStream(mainMenuMusic);
    UnloadMusicStream(victoryMusic);
    UnloadTexture(MainBg);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}