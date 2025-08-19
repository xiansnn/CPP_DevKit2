add_library(device_hc_sr04 hc_sr04.cpp)
target_link_libraries(device_hc_sr04 
            pico_stdlib
            )

add_library(device_rtos_hc_sr04 rtos_hc_sr04.cpp)
target_link_libraries(device_rtos_hc_sr04 
            pico_stdlib
            FreeRTOS-Kernel-Heap4 
            )

