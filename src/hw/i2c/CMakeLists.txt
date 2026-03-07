add_library(hw_i2c hw_i2c.cpp)
target_link_libraries(hw_i2c 
            pico_stdlib 
            hardware_i2c 
            pico_i2c_slave 
            )


