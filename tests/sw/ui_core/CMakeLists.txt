add_executable(test_ui_core  test_ui_core.cpp )               
target_link_libraries(test_ui_core 
            pico_stdlib
            device_ky040
            sw_widget
            sw_display_device
            utilities_probe
            )