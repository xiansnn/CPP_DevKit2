include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
add_executable( test_rtos_widget 
                test_rtos_widget.cpp
                t_rtos_widget_text_and_graph_widgets.cpp
                t_rtos_widget_encoder_controller.cpp
                t_rtos_widget_main_model_tasks.cpp
                t_rtos_widget_main_model_classes.cpp
                t_rtos_widget_ST7735_display_setup.cpp
                t_rtos_widget_SSD1306_display_setup.cpp
                )
target_link_libraries(test_rtos_widget 
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
           
