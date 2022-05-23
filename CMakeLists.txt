cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0076 NEW)

# set the project name
project(minetest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(minetest src/main.c)

target_sources(minetest PUBLIC src/block.c src/ray.c src/text.c  src/chunk.c src/world.c src/system.c)
target_sources(minetest PUBLIC src/math/vec.c src/math/matrix.c)
target_sources(minetest PUBLIC src/utils/font_loader.c src/utils/img_loader.c)
target_sources(minetest PUBLIC src/glx/atlas.c src/glx/texture.c src/glx/vao.c src/glx/vbo.c src/glx/shader.c)
target_sources(minetest PUBLIC src/blocks/wood.c src/blocks/glass.c src/blocks/granit.c src/blocks/plank.c src/blocks/cobble.c src/blocks/dirt.c src/blocks/grass.c src/blocks/stone.c)

target_include_directories(minetest PUBLIC include)
target_link_libraries(minetest -lm -lGLEW -lGL -lX11)
