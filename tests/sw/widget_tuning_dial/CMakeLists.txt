add_executable(test_tuning_dial test_tuning_dial.cpp
                t_tuning_dial_models.cpp
                t_tuning_dial_widgets.cpp
                )
target_include_directories(test_tuning_dial PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(test_tuning_dial
            pico_stdlib
            hw_i2c
            device_ssd1306
            sw_ui_core
            utilities_probe
            sw_widget
            )    
