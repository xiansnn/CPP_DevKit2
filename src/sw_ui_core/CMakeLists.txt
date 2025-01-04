add_library( ui_core 
            ui_display_device.cpp
            )
target_link_libraries(ui_core 
            pico_stdlib 
            framebuffer
            )
                        