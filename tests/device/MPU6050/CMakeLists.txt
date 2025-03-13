add_executable(test_mpu6050   test_mpu6050.cpp   )
target_link_libraries(test_mpu6050 
            pico_stdlib 
            device_mpu6050 
            hw_i2c 
            utilities_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_mpu6050_irq   test_mpu6050_irq.cpp   )
target_link_libraries(test_mpu6050_irq 
            pico_stdlib 
            device_mpu6050  
            hw_i2c 
            utilities_probe
            )