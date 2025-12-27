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
            rtos_widget
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
            device_ssd1306 
            utilities_probe
            FreeRTOS-Kernel-Heap4 
            )

add_subdirectory(test_rtos_widget_extended)

           
