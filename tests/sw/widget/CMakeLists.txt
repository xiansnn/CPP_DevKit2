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
            rtos_hw_spi 
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
#-------------------------------------------------------------------------------------
add_executable(test_rtos_st7735_graphic_primitives  test_rtos_st7735_graphic_primitives.cpp  )
target_link_libraries(test_rtos_st7735_graphic_primitives PUBLIC
            pico_stdlib 
            rtos_hw_spi
            device_st7735 
            rtos_widget
            utilities_probe
            
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_st7735_text_and_graph_widgets 
                test_rtos_st7735_text_and_graph_widgets.cpp
                t_rtos_st7735_text_and_graph_widgets.cpp
                )
target_link_libraries(test_rtos_st7735_text_and_graph_widgets 
            pico_stdlib 
            utilities_probe
            sw_ui_core
            rtos_ui_core
            rtos_hw_spi
            sw_widget
            device_st7735 
            sw_display_device
            
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_st7735_text_primitives test_rtos_st7735_text_primitives.cpp)
target_link_libraries(test_rtos_st7735_text_primitives 
            pico_stdlib 
            rtos_hw_spi
            device_st7735 
            rtos_widget
            utilities_probe
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_ssd1306_graphic_primitives  test_rtos_ssd1306_graphic_primitives.cpp  )
target_link_libraries(test_rtos_ssd1306_graphic_primitives 
            pico_stdlib 
            rtos_hw_i2c 
            device_ssd1306 
            # sw_widget
            rtos_widget
            # sw_ui_core
            sw_display_device
            utilities_probe
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_ssd1306_text_primitives test_rtos_ssd1306_text_primitives.cpp)
target_link_libraries(test_rtos_ssd1306_text_primitives 
            pico_stdlib 
            rtos_hw_i2c 
            rtos_widget
            # sw_ui_core
            device_ssd1306 
            utilities_probe
            FreeRTOS-Kernel-Heap4 
            )
            
