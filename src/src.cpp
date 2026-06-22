#include "raylib.h"
#include <vector>
#include <stack>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "raymath.h"

struct Cell {
    bool walls[4]; // N, E, S, W
    bool visited;
    Cell() { walls[0]=walls[1]=walls[2]=walls[3]=true; visited=false; }
};

// Simple sphere vs AABB check (returns true if sphere intersects box)
static bool CheckCollisionSphereBox(Vector3 center, float radius, BoundingBox box)
{
    // find closest point on AABB to sphere center
    float cx = fmaxf(box.min.x, fminf(center.x, box.max.x));
    float cy = fmaxf(box.min.y, fminf(center.y, box.max.y));
    float cz = fmaxf(box.min.z, fminf(center.z, box.max.z));

    float dx = center.x - cx;
    float dy = center.y - cy;
    float dz = center.z - cz;

    float dist2 = dx*dx + dy*dy + dz*dz;
    return dist2 <= (radius * radius);
}

int main()
{
    InitWindow(1280, 760, "MazeO - First Person Maze (raylib)");
    SetTargetFPS(60);

    // Maze parameters
    const int ROWS = 15;
    const int COLS = 15;
    const float CELL = 2.0f;
    const float WALL_THICK = 0.2f;
    const float WALL_H = 2.0f;

    // Player
    const float PLAYER_HEIGHT = 1.2f;
    const float PLAYER_RADIUS = 0.25f;
    float yaw = 0.0f, pitch = 0.0f;

    // Camera (first-person)
    Camera3D camera = { 0 };
    camera.position = { CELL*0.5f, PLAYER_HEIGHT, CELL*0.5f };
    camera.target = { camera.position.x + 0.0f, camera.position.y + 0.0f, camera.position.z + 1.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Build and carve maze (recursive backtracker)
    std::srand((unsigned)std::time(nullptr));
    std::vector<Cell> grid(ROWS * COLS);
    auto idx = [&](int r, int c){ return r * COLS + c; };

    // stack for backtracking
    std::stack<std::pair<int,int>> st;
    int cr = 0, cc = 0;
    grid[idx(cr,cc)].visited = true;

    auto hasUnvisited = [&](int r,int c){
        return (r>=0 && r<ROWS && c>=0 && c<COLS && !grid[idx(r,c)].visited);
    };

    while (true) {
        std::vector<std::pair<int,int>> nbs;
        // N
        if (hasUnvisited(cr-1,cc)) nbs.emplace_back(cr-1,cc);
        // E
        if (hasUnvisited(cr,cc+1)) nbs.emplace_back(cr,cc+1);
        // S
        if (hasUnvisited(cr+1,cc)) nbs.emplace_back(cr+1,cc);
        // W
        if (hasUnvisited(cr,cc-1)) nbs.emplace_back(cr,cc-1);

        if (!nbs.empty()) {
            auto pick = nbs[std::rand() % nbs.size()];
            st.emplace(cr,cc);
            int nr = pick.first, nc = pick.second;
            // remove wall between (cr,cc) and (nr,nc)
            if (nr == cr-1) { // north
                grid[idx(cr,cc)].walls[0] = false;
                grid[idx(nr,nc)].walls[2] = false;
            } else if (nc == cc+1) { // east
                grid[idx(cr,cc)].walls[1] = false;
                grid[idx(nr,nc)].walls[3] = false;
            } else if (nr == cr+1) { // south
                grid[idx(cr,cc)].walls[2] = false;
                grid[idx(nr,nc)].walls[0] = false;
            } else if (nc == cc-1) { // west
                grid[idx(cr,cc)].walls[3] = false;
                grid[idx(nr,nc)].walls[1] = false;
            }
            cr = nr; cc = nc;
            grid[idx(cr,cc)].visited = true;
        } else if (!st.empty()) {
            auto p = st.top(); st.pop(); cr = p.first; cc = p.second;
        } else break;
    }

    // Build wall bounding boxes and store wall draw data
    struct Wall { Vector3 center; Vector3 size; BoundingBox box; };
    std::vector<Wall> walls;

    for (int r=0;r<ROWS;r++){
        for (int c=0;c<COLS;c++){
            Cell &cell = grid[idx(r,c)];
            float x = c * CELL;
            float z = r * CELL;
            // North
            if (cell.walls[0]){
                Vector3 center = { x + CELL*0.5f, WALL_H*0.5f, z };
                Vector3 size = { CELL, WALL_H, WALL_THICK };
                Wall w; w.center = center; w.size = size;
                w.box.min = { center.x - size.x*0.5f, center.y - size.y*0.5f, center.z - size.z*0.5f };
                w.box.max = { center.x + size.x*0.5f, center.y + size.y*0.5f, center.z + size.z*0.5f };
                walls.push_back(w);
            }
            // East
            if (cell.walls[1]){
                Vector3 center = { x + CELL, WALL_H*0.5f, z + CELL*0.5f };
                Vector3 size = { WALL_THICK, WALL_H, CELL };
                Wall w; w.center = center; w.size = size;
                w.box.min = { center.x - size.x*0.5f, center.y - size.y*0.5f, center.z - size.z*0.5f };
                w.box.max = { center.x + size.x*0.5f, center.y + size.y*0.5f, center.z + size.z*0.5f };
                walls.push_back(w);
            }
            // South
            if (cell.walls[2]){
                Vector3 center = { x + CELL*0.5f, WALL_H*0.5f, z + CELL };
                Vector3 size = { CELL, WALL_H, WALL_THICK };
                Wall w; w.center = center; w.size = size;
                w.box.min = { center.x - size.x*0.5f, center.y - size.y*0.5f, center.z - size.z*0.5f };
                w.box.max = { center.x + size.x*0.5f, center.y + size.y*0.5f, center.z + size.z*0.5f };
                walls.push_back(w);
            }
            // West
            if (cell.walls[3]){
                Vector3 center = { x, WALL_H*0.5f, z + CELL*0.5f };
                Vector3 size = { WALL_THICK, WALL_H, CELL };
                Wall w; w.center = center; w.size = size;
                w.box.min = { center.x - size.x*0.5f, center.y - size.y*0.5f, center.z - size.z*0.5f };
                w.box.max = { center.x + size.x*0.5f, center.y + size.y*0.5f, center.z + size.z*0.5f };
                walls.push_back(w);
            }
        }
    }

    // Center the maze a bit by offsetting everything so it sits near origin
    const float mazeWidth = COLS * CELL;
    const float mazeDepth = ROWS * CELL;
    const Vector3 mazeOffset = { -mazeWidth*0.5f + CELL*0.5f, 0.0f, -mazeDepth*0.5f + CELL*0.5f };
    for (auto &w : walls) {
        w.center.x += mazeOffset.x; w.center.y += mazeOffset.y; w.center.z += mazeOffset.z;
        w.box.min.x += mazeOffset.x; w.box.min.y += mazeOffset.y; w.box.min.z += mazeOffset.z;
        w.box.max.x += mazeOffset.x; w.box.max.y += mazeOffset.y; w.box.max.z += mazeOffset.z;
    }
    camera.position.x += mazeOffset.x; camera.position.z += mazeOffset.z;

    // Mouse capture for look
    // Use DisableCursor to hide and capture the cursor in raylib versions without SetMouseMode
    DisableCursor();

    // Main loop
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Mouse look
        Vector2 md = GetMouseDelta();
        const float sensitivity = 0.0035f; // radians per pixel
        yaw += md.x * sensitivity;
        pitch += -md.y * sensitivity;
        if (pitch > 1.47f) pitch = 1.47f;
        if (pitch < -1.47f) pitch = -1.47f;

        // forward vector from yaw/pitch
        Vector3 forward;
        forward.x = cosf(pitch) * sinf(yaw);
        forward.y = sinf(pitch);
        forward.z = cosf(pitch) * cosf(yaw);
        // flatten for movement (no vertical walking)
        Vector3 flat = { forward.x, 0.0f, forward.z };
        float len = sqrtf(flat.x*flat.x + flat.y*flat.y + flat.z*flat.z);
        if (len != 0.0f) {
            flat.x /= len; flat.y = 0; flat.z /= len;
        }
        // right vector
        Vector3 right = Vector3Normalize(Vector3CrossProduct(flat, camera.up));

        // movement input
        float speed = 4.0f;
        if (IsKeyDown(KEY_LEFT_SHIFT)) speed = 7.0f;
        Vector3 move = { 0, 0, 0 };
        if (IsKeyDown(KEY_W)) { move.x += flat.x * speed * dt; move.z += flat.z * speed * dt; }
        if (IsKeyDown(KEY_S)) { move.x -= flat.x * speed * dt; move.z -= flat.z * speed * dt; }
        if (IsKeyDown(KEY_A)) { move.x -= right.x * speed * dt; move.z -= right.z * speed * dt; }
        if (IsKeyDown(KEY_D)) { move.x += right.x * speed * dt; move.z += right.z * speed * dt; }

        // collision: sphere vs boxes, do sliding by axis
        Vector3 newPos = camera.position;
        // attempt Z (forward/back)
        Vector3 attempt = newPos; attempt.x += move.x; attempt.z += move.z; // attempt full
        bool blocked = false;
        for (auto &w : walls) {
            if (CheckCollisionSphereBox(attempt, PLAYER_RADIUS, w.box)) { blocked = true; break; }
        }
        if (!blocked) {
            newPos = attempt;
        } else {
            // try X only
            Vector3 attemptX = camera.position; attemptX.x += move.x; attemptX.z = camera.position.z;
            bool blockX = false;
            for (auto &w : walls) { if (CheckCollisionSphereBox(attemptX, PLAYER_RADIUS, w.box)) { blockX = true; break; } }
            if (!blockX) newPos = attemptX;
            else {
                // try Z only
                Vector3 attemptZ = camera.position; attemptZ.x = camera.position.x; attemptZ.z += move.z;
                bool blockZ = false;
                for (auto &w : walls) { if (CheckCollisionSphereBox(attemptZ, PLAYER_RADIUS, w.box)) { blockZ = true; break; } }
                if (!blockZ) newPos = attemptZ;
            }
        }

        camera.position.x = newPos.x;
        camera.position.z = newPos.z;
        camera.target.x = camera.position.x + forward.x;
        camera.target.y = camera.position.y + forward.y;
        camera.target.z = camera.position.z + forward.z;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        // floor
        Vector3 planePos = { 0.0f, 0.0f, 0.0f };
        Vector2 planeSize = { mazeWidth, mazeDepth };
        DrawPlane(planePos, planeSize, LIGHTGRAY);

        // draw maze walls
        for (auto &w : walls) {
            DrawCube(w.center, w.size.x, w.size.y, w.size.z, DARKGRAY);
        }

        EndMode3D();

        // HUD
        DrawText("WASD to move, mouse to look, SHIFT to run", 10, 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
