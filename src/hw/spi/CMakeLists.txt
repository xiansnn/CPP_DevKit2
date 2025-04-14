add_library(hw_spi hw_spi.cpp)
target_link_libraries(hw_spi 
            pico_stdlib 
            hardware_spi 
            )


