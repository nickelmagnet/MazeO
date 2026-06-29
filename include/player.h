#pragma once
#include "raylib.h"
#include "maze.h"
#include <vector>

struct Player {
    Camera3D camera;
    float yaw;
    float pitch;

    static constexpr float HEIGHT = 1.2f;
    static constexpr float RADIUS = 0.25f;
    static constexpr float SPEED  = 4.0f;
    static constexpr float SPRINT = 7.0f;
};


Player CreatePlayer(int spawnRow, int spawnCol, const Maze& maze);

// Handles mouse look + WASD movement + wall collision
void UpdatePlayer(Player& player, const std::vector<Wall>& walls, float dt,float sensitivity);
