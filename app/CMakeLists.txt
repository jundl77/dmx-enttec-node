add_executable(dmx_hue_node_app
        app.cc
        app.h
        main.cc)

target_link_libraries(dmx_hue_node_app
        dmx_hue_node_config_lib
        dmx_hue_node_server_lib)

find_package(Boost 1.53 REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
target_link_libraries(dmx_hue_node_app
        ${Boost_LIBRARIES})

install(TARGETS dmx_hue_node_app DESTINATION /usr/bin)