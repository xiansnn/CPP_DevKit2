add_executable(test_rtos_queue test_rtos_queue.cpp)
target_link_libraries(test_rtos_queue
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            utilities_probe
)