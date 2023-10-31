cmake_minimum_required(VERSION 3.16)
project(vendorlibs)

add_library(vendorlibs ${CMAKE_CURRENT_SOURCE_DIR}/nuklear/nuklear.h ${CMAKE_CURRENT_SOURCE_DIR}/nuklear/demo/sdl_renderer/nuklear_sdl_renderer.h)
set_target_properties(vendorlibs PROPERTIES LINKER_LANGUAGE C)
target_include_directories(vendorlibs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nuklear)