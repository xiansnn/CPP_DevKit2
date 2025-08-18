add_library(device_rotary_encoder rotary_encoder.cpp)
target_link_libraries(device_rotary_encoder 
            pico_stdlib 
            device_switch_button
            )

add_library(device_rtos_rotary_encoder rtos_rotary_encoder.cpp)
target_link_libraries(device_rtos_rotary_encoder 
            pico_stdlib 
            device_rtos_switch_button
            FreeRTOS-Kernel-Heap4 
            )