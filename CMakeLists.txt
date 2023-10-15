cmake_minimum_required(VERSION 3.5.1)

project(dmx_enttec_node LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(cxx_compile_options -Wall -Wextra)
endif()

include(cmake/FindFTD2XX.cmake)

add_subdirectory(x_njson)
add_subdirectory(x_libsoundio)

add_subdirectory(libs)
add_subdirectory(app_receiver_node)
add_subdirectory(app_audio_sender)
