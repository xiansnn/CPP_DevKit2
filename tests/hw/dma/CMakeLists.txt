add_executable(test_dma_m2m mem2mem.cpp)
target_link_libraries(test_dma_m2m 
            pico_stdlib 
            hw_dma
            utilities_probe
            )

add_executable(test_dma_m2m_via_spi mem2mem_via_spi.cpp)
target_link_libraries(test_dma_m2m_via_spi 
            pico_stdlib
            FreeRTOS-Kernel-Heap4 
            hw_dma
            rtos_hw_spi 
            utilities_probe
            )

add_executable(test_dma_m2m_via_i2c mem2mem_via_i2c.cpp)
target_link_libraries(test_dma_m2m_via_i2c 
            pico_stdlib
            FreeRTOS-Kernel-Heap4
            hw_dma
            rtos_hw_i2c
            utilities_probe
            )

