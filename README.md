# MazeO

A first-person 3D maze game built from scratch in C++ using [raylib](https://www.raylib.com/).

Procedurally generated mazes, five difficulty levels, themeable visuals and music, and a menu system — all built with raw raylib draw calls and no game engine.

## Features

- **Procedural maze generation** using a recursive backtracker algorithm
- **First-person 3D movement** with mouse look and sliding wall collision
- **5 difficulty levels** (9x9 up to 33x33), randomized spawn and exit points each run
- **5 visual themes** (Default, Winter, Nether, End, Summer) with their own wall/floor textures(32×32 pixel art), sky color, fog density, and background music
- **Full menu system**: main menu, difficulty select, pause menu, settings, and level complete screen — all keyboard and mouse navigable, Minecraft inspired button visuals
- **Settings**: FOV, mouse sensitivity, max Framerate (including unlimited), music volume — all adjustable with sliders and  theme selection
- **Looping background music per theme**, plus dedicated main menu and victory tracks

## Controls

| Action | Key |
|---|---|
| Move | `W` `A` `S` `D` |
| Look | Mouse |
| Sprint | `Left Shift` |
| Pause / Back | `Esc` |
| Menu navigation | `↑` `↓` `←` `→` `Enter` |

## Project Structure

```
src/
├── main.cpp      # game loop, state machine, rendering
├── maze.cpp      # maze generation (recursive backtracker) + wall geometry
├── player.cpp    # first-person camera, movement, collision 
└── ui.cpp        # all menus, buttons, sliders

include/         
├── maze.h 
├── player.h
└── ui.h

assets/
├── themes/
│   ├── default/  winter/  nether/  end/  summer/
│   │   ├── wall.png
│   │   ├── floor.png
|   |   ├── orb.png
│   │   └── music.ogg
└── main/
    ├── menu_music.ogg
    ├── victory_music.ogg
    └── pause_bg.png
    
```

## Building

Requires [raylib](https://github.com/raysan5/raylib) installed and linked.

This project uses CMake.

To build it:

- Configure Raylib on your system.
- Add the `include/` folder to your include directories.
- Compile all `.cpp` files inside `src/`.
- Link against Raylib

(Adjust linker flags for your platform — see raylib's build instructions for Windows/Linux/macOS specifics.)

## How It Works

The maze is generated with a **recursive backtracker**: starting from cell (0,0), the algorithm carves a path through unvisited neighboring cells, pushing visited cells onto a stack and backtracking when it hits a dead end, until every cell has been visited. This guarantees a maze with exactly one path between any two points.

Each maze cell stores which of its four walls (N/E/S/W) are still standing. After generation, standing walls are converted into 3D bounding boxes used both for rendering and for sphere-vs-box collision against the player.

The player is a first-person camera using raylib's `Camera3D`, with mouse-look yaw/pitch and WASD movement projected onto the camera's forward/right vectors. Collision uses axis-sliding: if the player can't move on both axes, it tries X-only then Z-only, so players slide along walls instead of stopping dead.

## Status

In development. Built as a personal project to learn C++ and raylib.
