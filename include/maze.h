#pragma once
#include "raylib.h"
#include <vector>

// ─────────────────────────────────────
//  MAZE
// ─────────────────────────────────────

struct Cell {
    bool walls[4]; // N, E, S, W
    bool visited;
    Cell() { walls[0]=walls[1]=walls[2]=walls[3]=true; visited=false; }
};

struct Wall {
    Vector3 center;
    Vector3 size;
    BoundingBox box;
};

struct Maze {
    int rows, cols;
    float cellSize;
    float wallThick;
    float wallH;
    float width;   // cols * cellSize
    float depth;   // rows * cellSize
    std::vector<Cell> grid;
    std::vector<Wall> walls;
};

// Generates maze grid (recursive backtracker) and builds wall list.
// mazeOffset is applied so maze is centered around origin.
Maze GenerateMaze(int rows, int cols, float cellSize, float wallThick, float wallH);
