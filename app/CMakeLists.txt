add_executable(dmx_enttec_node_app
        app.cc
        app.h
        main.cc)

target_link_libraries(dmx_enttec_node_app
        dmx_enttec_node_config_lib
        dmx_enttec_node_server_lib)

target_link_libraries(dmx_enttec_node_app)

install(TARGETS dmx_enttec_node_app DESTINATION /usr/bin)