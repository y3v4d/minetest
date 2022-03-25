cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0076 NEW)

# set the project name
project(minetest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(minetest src/main.c)

target_sources(
    minetest 
    PUBLIC 
    src/atlas.c src/utils/font_loader.c src/chunk.c src/glx/vao.c src/glx/vbo.c src/shader.c src/system.c src/utils/img_loader.c src/math/vec.c src/math/matrix.c
)
target_include_directories(minetest PUBLIC include)
target_link_libraries(minetest -lm -lGLEW -lGL -lX11)
