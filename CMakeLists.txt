cmake_minimum_required(VERSION 3.16)
project(ChessEngine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
add_compile_options(-O3 -march=native)

file(GLOB_RECURSE ENGINE_SOURCES src/*.cpp)
list(REMOVE_ITEM ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(chess src/main.cpp ${ENGINE_SOURCES})
target_include_directories(chess PRIVATE src)

# Perft test binary
add_executable(perft tests/perft_test.cpp ${ENGINE_SOURCES})
target_include_directories(perft PRIVATE src)
