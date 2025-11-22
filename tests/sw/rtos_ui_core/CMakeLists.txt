add_executable(test_rtos_ui_core  
            test_rtos_ui_core.cpp
            t_rtos_manager.cpp 
            t_rtos_controlled_value.cpp
            t_rtos_widget_on_serial_monitor.cpp
            )               
target_link_libraries(test_rtos_ui_core 
            pico_stdlib
            # rtos_hw_spi
            sw_ui_core
            rtos_ui_core
            dev_rtos_rotary_encoder
            # device_st7735
            sw_widget
            rtos_widget
            sw_display_device
            utilities_probe
            FreeRTOS-Kernel-Heap4
            )