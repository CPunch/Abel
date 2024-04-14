cmake_minimum_required(VERSION 3.16)
project(libcore LANGUAGES C)

# ======================================= [[ GLOB SOURCE ]] =======================================

set(INCLUDEDIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/**.c)
file(GLOB_RECURSE HEADERS ${INCLUDEDIR}/**.h)

# ====================================== [[ BUILD TARGET ]] =======================================

add_library(libcore ${SOURCE} ${HEADERS})

# add the 'DEBUG' preprocessor definition if we're compiling as Debug
target_compile_definitions(libcore PUBLIC "$<$<CONFIG:Debug>:DEBUG>")

# add -lm if unix to link the math lib
if(UNIX)
  target_link_libraries(libcore PUBLIC m)

  # add address sanitizer if we're compiling as Debug
  target_compile_options(libcore PUBLIC "$<$<CONFIG:Debug>:-fsanitize=address>")
  target_link_options(libcore PUBLIC "$<$<CONFIG:Debug>:-fsanitize=address>")
endif()

# add ABEL_ASAN define if USE_ASAN is set
if(USE_ASAN)
  target_compile_definitions(libcore PUBLIC ABEL_ASAN)
endif()

# add include directory
target_include_directories(libcore
  PUBLIC
  ${INCLUDEDIR}
)

# link libraries
target_link_libraries(libcore PUBLIC
  $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
  $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
  $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
  $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
  $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
  zip
  vendorlibs
)

add_subdirectory(test)
add_test(NAME libcore COMMAND "$<TARGET_FILE:libcoretest>")