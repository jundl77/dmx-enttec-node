set(name dmx_hue_node_core_lib)
set(sources
        logger.h
        njson.h
        strong_typedef.h
        tsc_clock.h
        throw_if.h
        types.cc
        types.h
        )

add_library(${name} ${sources})

target_link_libraries(${name}
        huestream
        )

find_package(Boost 1.53 REQUIRED system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_link_libraries(${name} ${Boost_LIBRARIES} stdc++fs)
else()
    target_link_libraries(${name} ${Boost_LIBRARIES})
endif()

target_compile_options(${name} PRIVATE ${cxx_compile_options})
target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})