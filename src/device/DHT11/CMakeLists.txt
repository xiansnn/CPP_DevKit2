add_library(device_dht11 dht11.cpp)
target_link_libraries(device_dht11 
            pico_stdlib)


add_library(device_rtos_dht11 rtos_dht11.cpp)
target_link_libraries(device_rtos_dht11 
            FreeRTOS-Kernel-Heap4 
            pico_stdlib)


