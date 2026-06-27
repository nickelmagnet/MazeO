#include "maze.h"
#include "raylib.h"
#include <stack>
#include <vector>

Maze GenerateMaze(int rows, int cols, float cellSize, float wallThick, float wallH)
{
    Maze maze;
    maze.rows      = rows;
    maze.cols      = cols;
    maze.cellSize  = cellSize;
    maze.wallThick = wallThick;
    maze.wallH     = wallH;
    maze.width     = cols * cellSize;
    maze.depth     = rows * cellSize;
    maze.grid.resize(rows * cols);

    auto idx = [&](int r, int c){ return r * cols + c; };

    // ── Recursive backtracker ──
    std::stack<std::pair<int,int>> st;
    int cr = 0, cc = 0;
    maze.grid[idx(cr,cc)].visited = true;

    auto CanVisit = [&](int r, int c){
        return (r>=0 && r<rows && c>=0 && c<cols && !maze.grid[idx(r,c)].visited);
    };

    while (true) {
        std::vector<std::pair<int,int>> nbs;
        if (CanVisit(cr-1, cc)) nbs.emplace_back(cr-1, cc); // N
        if (CanVisit(cr, cc+1)) nbs.emplace_back(cr, cc+1); // E
        if (CanVisit(cr+1, cc)) nbs.emplace_back(cr+1, cc); // S
        if (CanVisit(cr, cc-1)) nbs.emplace_back(cr, cc-1); // W

        if (!nbs.empty()) {
            auto pick = nbs[GetRandomValue(0, (int)nbs.size()-1)];
            st.emplace(cr, cc);
            int nr = pick.first, nc = pick.second;

            if      (nr == cr-1) { maze.grid[idx(cr,cc)].walls[0]=false; maze.grid[idx(nr,nc)].walls[2]=false; } // N
            else if (nc == cc+1) { maze.grid[idx(cr,cc)].walls[1]=false; maze.grid[idx(nr,nc)].walls[3]=false; } // E
            else if (nr == cr+1) { maze.grid[idx(cr,cc)].walls[2]=false; maze.grid[idx(nr,nc)].walls[0]=false; } // S
            else if (nc == cc-1) { maze.grid[idx(cr,cc)].walls[3]=false; maze.grid[idx(nr,nc)].walls[1]=false; } // W

            cr = nr; cc = nc;
            maze.grid[idx(cr,cc)].visited = true;
        } else if (!st.empty()) {
            auto p = st.top(); st.pop(); cr = p.first; cc = p.second;
        } else break;
    }

    // ── Build wall geometry ──
    const float C  = cellSize;
    const float WT = wallThick;
    const float WH = wallH;

    auto makeWall = [&](Vector3 center, Vector3 size) -> Wall {
        Wall w;
        w.center = center;
        w.size   = size;
        w.box.min = { center.x - size.x*0.5f, center.y - size.y*0.5f, center.z - size.z*0.5f };
        w.box.max = { center.x + size.x*0.5f, center.y + size.y*0.5f, center.z + size.z*0.5f };
        return w;
    };

    for (int r=0; r<rows; r++) {
        for (int c=0; c<cols; c++) {
            Cell& cell = maze.grid[idx(r,c)];
            float x = c * C;
            float z = r * C;
            if (cell.walls[0]) maze.walls.push_back(makeWall({ x+C*0.5f, WH*0.5f, z       }, { C,  WH, WT })); // N
            if (cell.walls[1]) maze.walls.push_back(makeWall({ x+C,      WH*0.5f, z+C*0.5f}, { WT, WH, C  })); // E
            if (cell.walls[2]) maze.walls.push_back(makeWall({ x+C*0.5f, WH*0.5f, z+C     }, { C,  WH, WT })); // S
            if (cell.walls[3]) maze.walls.push_back(makeWall({ x,        WH*0.5f, z+C*0.5f}, { WT, WH, C  })); // W
        }
    }

    // ── Center maze around origin ──
    float ox = -maze.width  * 0.5f;
    float oz = -maze.depth  * 0.5f;
    for (auto& w : maze.walls) {
        w.center.x += ox; w.center.z += oz;
        w.box.min.x += ox; w.box.min.z += oz;
        w.box.max.x += ox; w.box.max.z += oz;
    }

    return maze;
}
