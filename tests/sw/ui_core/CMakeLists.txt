add_executable(test_ui_core  test_ui_core.cpp
            t_controlled_value.cpp
            t_manager.cpp
            t_widget_on_serial_monitor.cpp 
            ) 
target_include_directories(test_ui_core PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)              
target_link_libraries(test_ui_core 
            pico_stdlib
            device_ky040
            sw_widget
            sw_display_device
            utilities_probe
            )