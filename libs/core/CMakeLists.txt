set(name dmx_enttec_node_core_lib)

file(GLOB sources *.cc *.h)

add_library(${name} ${sources})

find_package(Boost 1.53 REQUIRED system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_link_libraries(${name} ${Boost_LIBRARIES} stdc++fs)
else()
    target_link_libraries(${name} ${Boost_LIBRARIES})
endif()

target_compile_options(${name} PRIVATE ${cxx_compile_options})
target_include_directories(${name} PUBLIC ..)