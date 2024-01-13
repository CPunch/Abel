cmake_minimum_required(VERSION 3.16)
project(libcoretest LANGUAGES C)

file(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/**.c)

add_executable(libcoretest ${SOURCE})
target_link_libraries(libcoretest PRIVATE libcore)
