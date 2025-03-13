add_executable(test_i2c_bus_scan test_i2c_bus_scan.cpp)
target_link_libraries(test_i2c_bus_scan 
            hw_i2c 
            utilities_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_i2c_xchange test_i2c_xchange.cpp)
target_link_libraries(test_i2c_xchange 
            hw_i2c 
            utilities_probe
            )