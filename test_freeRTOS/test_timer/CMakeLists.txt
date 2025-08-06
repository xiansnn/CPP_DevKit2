add_executable(test_rtos_timer test_rtos_timer.cpp)
target_link_libraries(test_rtos_timer
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            utilities_probe
)