cmake_minimum_required(VERSION 3.12)
cmake_policy(SET CMP0076 NEW)

# set the project name
project(voxelly)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(voxelly src/main.c)

target_sources(voxelly PUBLIC src/sys/system.c)
target_sources(voxelly PUBLIC src/glx/atlas.c src/glx/texture.c src/glx/vao.c src/glx/vbo.c src/glx/shader.c)
target_sources(voxelly PUBLIC src/utils/stoper.c src/utils/font_loader.c src/utils/img_loader.c src/utils/vec.c src/utils/matrix.c)
target_sources(voxelly PUBLIC src/debugmenu.c src/game.c src/player.c src/highlight.c src/sprite.c src/camera.c src/direction.c src/block.c src/ray.c src/text.c  src/chunk.c src/world.c src/mesh.c)
target_sources(voxelly PUBLIC src/blocks/wood.c src/blocks/glass.c src/blocks/granit.c src/blocks/plank.c src/blocks/cobble.c src/blocks/dirt.c src/blocks/grass.c src/blocks/stone.c)

target_include_directories(voxelly PUBLIC include)
target_link_libraries(voxelly -lm -lGLEW -lGL -lX11)
