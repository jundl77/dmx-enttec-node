cmake_minimum_required(VERSION 3.5.1)

project(dmx_enttec_node LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(cxx_compile_options -Wall -Wextra)
endif()

add_subdirectory(x_njson)

add_subdirectory(app)
add_subdirectory(libs)
