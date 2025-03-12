
add_library(device_mpu6050 mpu6050.cpp)
target_link_libraries(device_mpu6050 
            pico_stdlib
            hw_i2c )
