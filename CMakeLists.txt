cmake_minimum_required(VERSION 4.3)
project(MazeO LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(MazeO
	src/main.cpp
	src/player.cpp
        src/maze.cpp
	src/ui.cpp
)	
target_include_directories(MazeO PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/raylib-6.0_win64_msvc16/include"
)

target_link_directories(MazeO PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/raylib-6.0_win64_msvc16/lib"
)

target_link_libraries(MazeO PRIVATE 
    raylib
    winmm
    gdi32
    opengl32
    user32
    shell32
)

