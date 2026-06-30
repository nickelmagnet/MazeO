#pragma once
#include "raylib.h"
#include "maze.h"
#include <vector>

struct Player {
    Camera3D camera;
    float yaw;
    float pitch;

    static constexpr float HEIGHT = 1.6f;
    static constexpr float RADIUS = 0.25f;
    static constexpr float SPEED  = 5.0f;
    static constexpr float SPRINT = 10.0f;
};


Player CreatePlayer(int spawnRow, int spawnCol, const Maze& maze);

// Handles mouse look + WASD movement + wall collision
void UpdatePlayer(Player& player, const std::vector<Wall>& walls, float dt,float sensitivity);
