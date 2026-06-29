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
    SetTargetFPS(0);
    SetExitKey(KEY_NULL);

	GameSettings settings;

    const int difficulty[][2] = { {9,9},{15,15},{21,21},{27,27},{33,33} };
    int currentLevel = 0;

    // ── Maze + player (initialized on Play) ──
    Maze   maze = {};
    Player player = {};
    Vector3 exitPos = {};

    auto StartNewGame = [&](int level) {
        int rows = difficulty[level][0];
        int cols = difficulty[level][1];
        maze = GenerateMaze(rows, cols, 3.0f, 0.3f, 3.0f);

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
        bool levelComplete = false;
        };

    // ── State ──
    GameState state = GameState::MainMenu;
    EnableCursor();
    int pauseOpt = 0;
    GameState prevState = GameState::MainMenu; 
	

    //  GAME LOOP
    while (!WindowShouldClose())
    {   
        BeginDrawing();
        ClearBackground(Color{255,255,255,0});
        float dt = GetFrameTime();

        int frm = (settings.MaxFps = 241.0f) ? 0 : (int)settings.MaxFps;
        SetTargetFPS(frm);

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
            UpdatePlayer(player, maze.walls,dt,settings.sensitivity);

            float dx = player.camera.position.x - exitPos.x;
            float dz = player.camera.position.z - exitPos.z;
            float dist = sqrtf(dx * dx + dz * dz);
            if (dist < 0.8f) {
                state = GameState::LevelComplete;
                EnableCursor();
            }
        }
        // ─────────────────────────────
        //  DRAW
        // ─────────────────────────────
        if (state == GameState::MainMenu) {
            int action = DrawMainMenu();
            if (action == 1) {          // Choose Difficulty
                prevState = state;
                state = GameState::DifficultyMenu;
            }
            else if (action == 2) {   // Quit
                break;
            }
        }else if (state == GameState::DifficultyMenu) {
            int action = DrawDifficultyMenu();
            if (action > 0) {
                currentLevel = action-1;
                StartNewGame(currentLevel);
                state = GameState::Playing;
                DisableCursor();
            }else if (action == -1) {
                state = prevState;
            }
        }
        else if (state == GameState::Playing || state == GameState::Paused) {
            // ── 3D world ──
            BeginMode3D(player.camera);
            player.camera.fovy = settings.fov;
            DrawPlane({ 0,0,0 }, { maze.width, maze.depth }, LIGHTGRAY);
            DrawCube({ 0, 20.0f, 0 }, maze.width + 1100, 0.1f, maze.depth + 1100, SKYBLUE);
            for (auto& w : maze.walls)
                DrawCube(w.center, w.size.x, w.size.y, w.size.z, DARKGRAY);
            DrawSphere(exitPos, 0.4f, YELLOW);
            DrawSphereWires(exitPos, 0.42f, 6, 6, ORANGE);
            EndMode3D();

            // ── HUD ──
            DrawText("WASD: move  |  Mouse: look  |  Shift: sprint  |  Esc: pause", 10, 10, 18, BLACK);

            // ── Pause menu ──
            if (state == GameState::Paused) {
                int action = DrawPauseMenu(pauseOpt);
                if (action == 1) { state = GameState::Playing; DisableCursor(); }
                else if (action == 2) { prevState = GameState::Paused; state = GameState::Settings; }
                else if (action == 3) { state = GameState::MainMenu; EnableCursor(); }
            }
        }
        else if (state == GameState::LevelComplete) {
            int action = DrawLevelComplete();
            if (action == 1) {          // Play again
                StartNewGame(currentLevel);
                state = GameState::Playing;
                DisableCursor();
            }
            else if (action == 2) { prevState=state ;state = GameState::DifficultyMenu; }
            else if (action == 3) {  state = GameState::MainMenu;}
            else if (action == 4) {   // Quit
                break;
            }
        }
        else if (state == GameState::Settings) {
            int action = DrawSettingsMenu(settings);
            if (action == -1) {
                state = prevState;
				int frm = (settings.MaxFps >= 241.0f) ? 0 : (int)settings.MaxFps;
				SetTargetFPS(frm);
                if (prevState == GameState::Paused) EnableCursor();
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}