add_library(device_switch_button switch_button.cpp)
target_link_libraries(device_switch_button 
            pico_stdlib 
            )

add_library(device_rtos_switch_button rtos_switch_button.cpp)
target_link_libraries(device_rtos_switch_button 
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            )