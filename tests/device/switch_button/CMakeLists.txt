add_executable(test_switch_button test_switch_button_irq.cpp)
target_link_libraries(test_switch_button  
            pico_stdlib
            device_switch_button
            utilities_probe
            )
            #-------------------------------------------------------------------------------------
add_executable(test_gpio_irq test_gpio_irq.cpp )
target_link_libraries(test_gpio_irq 
            pico_stdlib
            )