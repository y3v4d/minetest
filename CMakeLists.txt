cmake_minimum_required(VERSION 3.10)

# set the project name
project(minetest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_executable
add_executable(minetest src/main.c src/shader.c src/system.c src/utils/img_loader.c)

target_include_directories(minetest PUBLIC include)
target_link_libraries(minetest -lm -lGLEW -lGL -lX11)
