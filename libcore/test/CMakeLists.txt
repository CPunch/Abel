cmake_minimum_required(VERSION 3.16)
project(libcoretest)

add_executable(libcoretest main.c)
target_link_libraries(libcoretest PRIVATE libcore)
