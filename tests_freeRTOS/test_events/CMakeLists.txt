add_executable(test_rtos_events test_rtos_events.cpp)
target_link_libraries(test_rtos_events
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            utilities_probe
)