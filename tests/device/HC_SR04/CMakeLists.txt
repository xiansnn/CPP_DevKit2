add_executable(test_hc_sr04 test_hc_sr04.cpp )
target_link_libraries(test_hc_sr04 
            pico_stdlib 
            device_hc_sr04
            )

add_executable(test_rtos_hc_sr04 test_rtos_hc_sr04.cpp )
target_link_libraries(test_rtos_hc_sr04 
            pico_stdlib 
            device_rtos_hc_sr04
            FreeRTOS-Kernel-Heap4 
            )