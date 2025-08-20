add_executable(test_rtos_semaphore test_rtos_semaphore.cpp)
target_link_libraries(test_rtos_semaphore
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            utilities_probe
)