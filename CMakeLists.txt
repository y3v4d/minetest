cmake_minimum_required(VERSION 3.10)

# set the project name
project(minetest)

# add_executable
add_executable(minetest main.c)

target_link_libraries(minetest -lm -lGLEW -lGL -lX11)
