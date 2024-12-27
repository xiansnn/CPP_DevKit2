
add_library(mpu6050 mpu6050.cpp)
target_link_libraries(mpu6050 
            pico_stdlib
            hardware_i2c 
            hw_i2c )
