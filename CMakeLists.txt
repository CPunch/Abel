cmake_minimum_required(VERSION 3.16)
project(Abel)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

execute_process(COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE GIT_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

# Set the project as the default startup project for VS
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Abel)

# Put CMake targets (ALL_BUILD/ZERO_CHECK) into a folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# include our modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules/")

# ==================================== [[ include libraries ]] ====================================

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_subdirectory(vendor)

# ======================================= [[ GLOB SOURCE ]] =======================================

set(INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/**.c)
file(GLOB_RECURSE HEADERS ${INCLUDEDIR}/**.h)

# ====================================== [[ BUILD TARGET ]] =======================================

add_executable(Abel ${SOURCE} ${HEADERS})

# add the 'DEBUG' preprocessor definition if we're compiling as Debug
target_compile_definitions(Abel PUBLIC "$<$<CONFIG:Debug>:DEBUG>")

# add -lm if unix to link the math lib
if(UNIX)
  target_link_libraries(Abel PUBLIC m)

  # add address sanitizer if we're compiling as Debug
  target_compile_options(Abel PUBLIC "$<$<CONFIG:Debug>:-fsanitize=address>")
  target_link_options(Abel PUBLIC "$<$<CONFIG:Debug>:-fsanitize=address>")
endif()

# add include directory
target_include_directories(Abel PUBLIC ${INCLUDEDIR} ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS})

# link libraries
target_link_libraries(Abel PUBLIC ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} vendorlibs)