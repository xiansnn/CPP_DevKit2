add_executable(test_tuning_dial test_tuning_dial.cpp)
target_link_libraries(test_tuning_dial
            pico_stdlib
            hw_i2c
            device_ssd1306
            sw_ui_core
            utilities_probe
            sw_widget
            )    
#-------------------------------------------------------------------------------------
add_executable(test_text_primitives test_text_primitives.cpp)
target_link_libraries(test_text_primitives 
            pico_stdlib 
            utilities_probe
            hw_i2c 
            sw_widget
            sw_ui_core
            device_ssd1306 
            )
#-------------------------------------------------------------------------------------
add_executable(test_graphic_primitives  test_graphic_primitives.cpp  )
target_link_libraries(test_graphic_primitives 
            device_ssd1306 
            pico_stdlib 
            hw_i2c 
            sw_widget
            sw_ui_core
            sw_display_device
            utilities_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_text_and_graph_widgets test_text_and_graph_widgets.cpp)
target_link_libraries(test_text_and_graph_widgets 
            pico_stdlib 
            utilities_probe
            sw_ui_core
            hw_i2c 
            sw_widget
            device_ssd1306 
            sw_display_device
            )
#-------------------------------------------------------------------------------------
