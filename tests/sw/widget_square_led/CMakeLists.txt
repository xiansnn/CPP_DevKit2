add_executable(test_square_led test_square_led.cpp
                    t_switch_button_controller.cpp
                    t_square_led_model.cpp
                    t_square_led_widget.cpp
                     )
target_include_directories(test_square_led PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(test_square_led 
            pico_stdlib
            sw_widget_square_led
            hw_i2c
            sw_ui_core
            utilities_probe
            device_ssd1306
            device_switch_button
            )
