
#-------------------------------------------------------------------------------------
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_executable(test_rtos_blinking_led 
                test_rtos_blinking_led.cpp
                t_rtos_blinking_led_display_setup.cpp
                t_rtos_blinking_led_encoder_controller.cpp
                t_rtos_blinking_led_main_classes.cpp
                t_rtos_blinking_led_main_tasks.cpp
                t_rtos_blinking_led_dummy_widgets.cpp
                t_rtos_blinking_led_monitoring_widgets.cpp
                t_rtos_blinking_led_main_widgets.cpp
                )
target_link_libraries(test_rtos_blinking_led 
            pico_stdlib 
            utilities_probe
            rtos_ui_core
            # rtos_hw_spi
            # rtos_hw_i2c
            rtos_widget
            device_ssd1306
            device_st7735 
            dev_rtos_rotary_encoder
            # sw_display_device
            FreeRTOS-Kernel-Heap4
            )
           
