add_library(hw_dma hw_dma.cpp)
target_link_libraries(hw_dma 
            pico_stdlib 
            hardware_dma 
            hardware_spi
            hw_i2c
            )


