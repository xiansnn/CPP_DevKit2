add_executable(test_horizontal_bargraph test_horizontal_bargraph.cpp)
target_link_libraries(test_horizontal_bargraph
            pico_stdlib
            hw_i2c
            device_ssd1306
            sw_ui_core
            utilities_probe
            sw_widget_bargraph
            sw_widget
            )    
#-------------------------------------------------------------------------------------
add_executable(test_vertical_bargraph test_vertical_bargraph.cpp)
target_link_libraries(test_vertical_bargraph
            pico_stdlib
            hw_i2c
            device_ssd1306
            sw_ui_core
            utilities_probe
            sw_widget_bargraph
            sw_widget
            )    