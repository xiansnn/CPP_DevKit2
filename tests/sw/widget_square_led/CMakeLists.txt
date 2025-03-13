add_executable(test_square_led test_square_led.cpp )
target_link_libraries(test_square_led 
            pico_stdlib
            sw_widget_square_led
            hw_i2c
            sw_ui_core
            utilities_probe
            device_ssd1306
            device_switch_button
            )
#-------------------------------------------------------------------------------------
add_executable(test_managed_square_led test_managed_square_led.cpp )
target_link_libraries(test_managed_square_led 
            pico_stdlib
            sw_widget_square_led
            hw_i2c
            sw_ui_core
            utilities_probe
            device_ssd1306
            device_ky040
            )