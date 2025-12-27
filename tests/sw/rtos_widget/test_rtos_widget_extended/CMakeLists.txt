
#-------------------------------------------------------------------------------------
add_executable(test_rtos_extended_text_and_graph_widgets 
                test_rtos_extended_text_and_graph_widgets.cpp
                t_rtos_extended_text_and_graph_widgets.cpp
                t_rtos_extended_roll_control.cpp
                )
target_link_libraries(test_rtos_extended_text_and_graph_widgets 
            pico_stdlib 
            utilities_probe
            rtos_ui_core
            rtos_hw_spi
            rtos_hw_i2c
            rtos_widget
            device_ssd1306
            device_st7735 
            dev_rtos_rotary_encoder
            sw_display_device
            FreeRTOS-Kernel-Heap4
            )
           
