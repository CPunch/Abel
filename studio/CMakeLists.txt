cmake_minimum_required(VERSION 3.16)
project(studio LANGUAGES C)

add_executable(studio main.c)

target_link_libraries(studio PRIVATE libcore)