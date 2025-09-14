add_library(hw_spi hw_spi.cpp)
target_link_libraries(hw_spi 
            pico_stdlib 
            hardware_spi 
            )


add_library(rtos_hw_spi rtos_hw_spi.cpp)
target_link_libraries(rtos_hw_spi 
            hw_spi
            hw_dma
            FreeRTOS-Kernel-Heap4  
            )
