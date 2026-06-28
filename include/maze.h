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
    float width;   
    float depth;   
    std::vector<Cell> grid;
    std::vector<Wall> walls;

    Vector3 CellCenter(int r, int c) const {
        float ox = -width * 0.5f;
        float oz = -depth * 0.5f;
        return { ox + (c + 0.5f) * cellSize, 0.0f, oz + (r + 0.5f) * cellSize };
    }
};

// Generates maze grid (recursive backtracker) and builds wall list.
// mazeOffset is applied so maze is centered around origin.
Maze GenerateMaze(int rows, int cols, float cellSize, float wallThick, float wallH);
