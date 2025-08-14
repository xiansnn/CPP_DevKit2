add_executable(test_rtos_gpio_irq test_rtos_gpio_irq.cpp )
target_link_libraries(test_rtos_gpio_irq 
            pico_stdlib
            FreeRTOS-Kernel-Heap4 
            utilities_probe
            )