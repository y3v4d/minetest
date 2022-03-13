cmake_minimum_required(VERSION 3.10)

# set the project name
project(minetest)

# add_executable
add_executable(minetest src/main.c src/shader.c)

target_include_directories(minetest PUBLIC include)
target_link_libraries(minetest -lm -lGLEW -lGL -lX11)