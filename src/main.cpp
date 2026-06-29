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
    SetTargetFPS(144);
    SetExitKey(KEY_NULL);

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

	

    //  GAME LOOP
    while (!WindowShouldClose())
    {   
        BeginDrawing();
        ClearBackground(Color{ 102, 191, 255,255 });
        float dt = GetFrameTime();

        // ── Escape toggles pause ──
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (state == GameState::Playing) {
                state = GameState::Paused;
                EnableCursor();
            } else if (state == GameState::Paused) {
                state = GameState::Playing;
                DisableCursor();
            }
            else if (state == GameState::MainMenu) {
                break;
            }
        }

        // ── Update ──
        if (state == GameState::Playing) {
            UpdatePlayer(player, maze.walls, dt);

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
                state = GameState::DifficultyMenu;
            }
            else if (action == 2) {   // Quit
                break;
            }
        }else if (state == GameState::DifficultyMenu) {
            int pick = DrawDifficultyMenu();
            if (pick > 0) {
                currentLevel = pick - 1;
                StartNewGame(currentLevel);
                state = GameState::Playing;
                DisableCursor();
            }
        }
        else if (state == GameState::Playing || state == GameState::Paused) {
            // ── 3D world ──
            BeginMode3D(player.camera);
            DrawPlane({ 0,0,0 }, { maze.width, maze.depth }, LIGHTGRAY);
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
                if (action == 1) {          // Resume
                    state = GameState::Playing;
                    DisableCursor();
                }
                else if (action == 2) {   // Settings
                    state = GameState::Settings;
                }
                else if (action == 3) {   // Quit to menu
                    state = GameState::MainMenu;
                    EnableCursor();
                }
            }
        }
        else if (state == GameState::LevelComplete) {
            int action = DrawLevelComplete();
            if (action == 1) {          // Play again
                StartNewGame(currentLevel);
                state = GameState::Playing;
                DisableCursor();
            }else if (action == 2) { state = GameState::DifficultyMenu; EnableCursor(); }
            else if (action == 3) {  state = GameState::MainMenu;  }
            else if (action == 4) {   // Quit
                break;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}