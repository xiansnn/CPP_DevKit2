add_executable(test_rtos_gpio_irq test_rtos_gpio_irq.cpp )
target_link_libraries(test_rtos_gpio_irq 
            pico_stdlib
            FreeRTOS-Kernel-Heap4 
            utilities_probe
            )
            
add_executable(test_rtos_switch_button test_rtos_switch_button.cpp)
target_link_libraries(test_rtos_switch_button  
            pico_stdlib
            FreeRTOS-Kernel-Heap4 
            device_switch_button
            utilities_probe
            )