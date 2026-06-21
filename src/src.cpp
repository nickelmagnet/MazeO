#include "raylib.h"

int main()
{
    InitWindow(1920, 1000, "BadhiyaWalaApp");
    Camera3D camera = { 0 };

    camera.position = { 2.0f, 2.0f, 2.0f };
    camera.target = { 0.0f, 1.0f, 0.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };

    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        UpdateCamera(&camera, CAMERA_FREE);
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        DrawPlane({ 0,0,0 }, { 20,20 }, GREEN);

        DrawCube({ 0,1,0 }, 2, 2, 2, RED);

        EndMode3D();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}