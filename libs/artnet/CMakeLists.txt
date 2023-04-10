set(name dmx_enttec_node_artnet_lib)

file(GLOB libartnet_sources libartnet/*.c libartnet/*.h)
file(GLOB sources *.cc *.h)

add_compile_options(-Wunused-variable -Wunused-parameter)

add_library(${name} ${sources} ${libartnet_sources})
target_link_libraries(${name}
        dmx_enttec_node_core_lib
        dmx_enttec_node_config_lib
        dmx_enttec_node_enttec_lib
        )

find_package(Boost 1.53 REQUIRED system)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    target_link_libraries(${name} ${Boost_LIBRARIES} stdc++fs)
else()
    target_link_libraries(${name} ${Boost_LIBRARIES})
endif()

target_compile_options(${name} PRIVATE ${cxx_compile_options})
target_include_directories(${name} PUBLIC ..)