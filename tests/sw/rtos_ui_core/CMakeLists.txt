add_executable(test_rtos_ui_core  test_rtos_ui_core.cpp )               
target_link_libraries(test_rtos_ui_core 
            pico_stdlib
            rtos_hw_spi
            # device_ky040
            device_st7735
            sw_widget
            sw_display_device
            utilities_probe
            FreeRTOS-Kernel-Heap4
            )