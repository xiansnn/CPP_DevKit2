add_executable(test_dma_hello hello_dma.cpp)
target_link_libraries(test_dma_hello 
            pico_stdlib 
            hardware_dma
            utilities_probe
            )

add_executable(test_dma_spi spi_dma.cpp)
target_link_libraries(test_dma_spi 
            pico_stdlib 
            hardware_dma
            hardware_spi
            utilities_probe
            )
