add_executable(test_rotary_encoder test_rotary_encoder.cpp)
target_link_libraries(test_rotary_encoder  
            pico_stdlib  
            device_rotary_encoder 
            utilities_probe 
            )

add_executable(test_rtos_rotary_encoder test_rtos_rotary_encoder.cpp)
target_link_libraries(test_rtos_rotary_encoder  
            pico_stdlib  
            dev_rtos_rotary_encoder 
            FreeRTOS-Kernel-Heap4 
            utilities_probe 
            )