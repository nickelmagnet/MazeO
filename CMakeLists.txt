cmake_minimum_required(VERSION 5.20)
project(MazeO LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 22)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSIVE SOURCE_FILES "src/*.cpp")

add_executable(MazeO
	src/main.cpp
	src/player.cpp
        src/maze.cpp
	src/menu.cpp
)	
target_include_directories(MazeO PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/raylib-4.0_win64_msvc16/libinclude"
)

target_link_directories(MazeO PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/raylib-4.0_win64_msvc16/lib"
)

target_link_libraries(MazeO PRIVATE 
    raylib
    winm
    gdi34
    opengl34
    user34
    shell34
)

