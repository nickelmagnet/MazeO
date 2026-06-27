#include "raylib.h"
#include "maze.h"
#include "player.h"
#include "ui.h"

// ─────────────────────────────────────
//  GAME STATES
// ─────────────────────────────────────
enum class GameState {
    Playing,
    Paused,
    Settings,
};

int main()
{
    InitWindow(1280, 760, "MazeO");
    SetTargetFPS(144);
    SetExitKey(KEY_NULL); // Esc handled manually

    // ── Generate maze ──
    Maze maze = GenerateMaze(15, 15, 3.0f, 0.3f, 3.0f);

    // ── Spawn player at (0,0) ──
    Player player = CreatePlayer(0, 0, maze);

    // ── State ──
    GameState state = GameState::Playing;
    int pauseOpt = 0;

    DisableCursor();

    // ─────────────────────────────────
    //  GAME LOOP
    // ─────────────────────────────────
    while (!WindowShouldClose())
    {
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
        }

        // ── Update ──
        if (state == GameState::Playing)
            UpdatePlayer(player, maze.walls, dt);

        // ─────────────────────────────
        //  DRAW
        // ─────────────────────────────
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // 3D world
        BeginMode3D(player.camera);
            DrawPlane({0,0,0}, {maze.width, maze.depth}, LIGHTGRAY);
            for (auto& w : maze.walls)
                DrawCube(w.center, w.size.x, w.size.y, w.size.z, DARKGRAY);
        EndMode3D();

        // HUD
        DrawText("WASD: move  |  Mouse: look  |  Shift: sprint  |  Esc: pause", 10, 10, 18, BLACK);

        // Pause menu
        if (state == GameState::Paused) {
            int action = DrawPauseMenu(pauseOpt);
            if (action == 1) {              // Resume
                state = GameState::Playing;
                DisableCursor();
            } else if (action == 2) {       // Settings (hook up later)
                state = GameState::Settings;
            } else if (action == 3) {       // Quit
                break;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
