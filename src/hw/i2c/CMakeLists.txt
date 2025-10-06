add_library(hw_i2c hw_i2c.cpp)
target_link_libraries(hw_i2c 
            pico_stdlib 
            hardware_i2c 
            pico_i2c_slave 
            )

add_library(rtos_hw_i2c rtos_hw_i2c.cpp)
target_link_libraries(rtos_hw_i2c 
            hw_i2c
            # FreeRTOS-Kernel
            hw_dma
            FreeRTOS-Kernel-Heap4
            # pico_stdlib 
            # hardware_i2c 
            # pico_i2c_slave 
            )
