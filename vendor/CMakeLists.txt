cmake_minimum_required(VERSION 3.16)
project(vendorlibs LANGUAGES C)

add_subdirectory(lua)

add_library(vendorlibs stub.c ${CMAKE_CURRENT_SOURCE_DIR}/nuklear/nuklear.h)
set_target_properties(vendorlibs PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(vendorlibs PUBLIC lua_static)
target_include_directories(vendorlibs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/nuklear)