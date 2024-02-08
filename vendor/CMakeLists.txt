cmake_minimum_required(VERSION 3.16)
project(vendorlibs LANGUAGES C)

set(LUA_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(LUA_ENABLE_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(lua)

add_library(vendorlibs stub.c)
set_target_properties(vendorlibs PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(vendorlibs PUBLIC lua_static)
target_include_directories(vendorlibs PUBLIC)