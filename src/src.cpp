#include "raylib.h"

int main()
{
    InitWindow(1920, 1080, "BadhiyaWalaApp");

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(PURPLE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}