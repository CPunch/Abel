cmake_minimum_required(VERSION 3.16)
project(Abel LANGUAGES C)

include(CTest)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(USE_ASAN "Use Address Sanitizer" OFF)

execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

# Set the project as the default startup project for VS
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT libcore)

# Put CMake targets (ALL_BUILD/ZERO_CHECK) into a folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (USE_ASAN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -g -fno-omit-frame-pointer -fsanitize=address")
endif()

# ==================================== [[ include libraries ]] ====================================

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(libzip REQUIRED)

enable_testing()

add_subdirectory(vendor)
add_subdirectory(libcore)
add_subdirectory(studio)
