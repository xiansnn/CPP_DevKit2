add_executable(test_horizontal_bar test_horizontal_bar.cpp)
target_link_libraries(test_horizontal_bar
            hw_i2c
            device_ssd1306
            sw_ui_core
            utilities_probe
            sw_widget_horizontal_bar
            )    
#-------------------------------------------------------------------------------------
add_executable(test_managed_horizontal_bar  test_managed_horizontal_bar.cpp)
target_link_libraries(test_managed_horizontal_bar
            pico_stdlib
            hw_i2c
            device_ssd1306
            sw_ui_core
            utilities_probe
            sw_widget_horizontal_bar
            sw_widget_square_led
            device_ky040
            )    