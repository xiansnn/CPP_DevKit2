add_executable(test_rtos_simple_notification test_rtos_simple_notification.cpp)
target_link_libraries(test_rtos_simple_notification
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            utilities_probe
)

add_executable(test_rtos_event_like_notification test_rtos_event_like_notification.cpp)
target_link_libraries(test_rtos_event_like_notification
            pico_stdlib 
            FreeRTOS-Kernel-Heap4 
            utilities_probe
)