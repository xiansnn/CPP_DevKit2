add_executable(test_ssd1306_text_primitives test_ssd1306_text_primitives.cpp)
target_link_libraries(test_ssd1306_text_primitives 
            pico_stdlib 
            utilities_probe
            hw_i2c 
            sw_widget
            sw_ui_core
            device_ssd1306 
            )
#-------------------------------------------------------------------------------------
add_executable(test_ssd1306_graphic_primitives  test_ssd1306_graphic_primitives.cpp  )
target_link_libraries(test_ssd1306_graphic_primitives 
            pico_stdlib 
            device_ssd1306 
            hw_i2c 
            sw_widget
            sw_ui_core
            sw_display_device
            utilities_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_ssd1306_text_and_graph_widgets test_ssd1306_text_and_graph_widgets.cpp)
target_link_libraries(test_ssd1306_text_and_graph_widgets 
            pico_stdlib 
            utilities_probe
            sw_ui_core
            hw_i2c 
            sw_widget
            device_ssd1306 
            sw_display_device
            )
#-------------------------------------------------------------------------------------
add_executable(test_st7735_graphic_primitives  test_st7735_graphic_primitives.cpp  )
target_link_libraries(test_st7735_graphic_primitives PUBLIC
            pico_stdlib 
            hw_spi 
            device_st7735 
            sw_widget
            utilities_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_st7735_text_primitives test_st7735_text_primitives.cpp)
target_link_libraries(test_st7735_text_primitives 
            pico_stdlib 
            hw_spi
            device_st7735 
            sw_widget
            utilities_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_st7735_text_and_graph_widgets test_st7735_text_and_graph_widgets.cpp)
target_link_libraries(test_st7735_text_and_graph_widgets 
            pico_stdlib 
            sw_ui_core
            hw_spi 
            sw_widget
            device_st7735 
            utilities_probe
            )

