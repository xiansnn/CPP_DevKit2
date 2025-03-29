add_executable(test_spi_master test_spi_master.cpp)
target_link_libraries(test_spi_master 
            hw_spi 
            hardware_spi
            utilities_probe
            )
#-------------------------------------------------------------------------------------
# add_executable(test_i2c_xchange test_i2c_xchange.cpp)
# target_link_libraries(test_i2c_xchange 
#             hw_i2c 
#             utilities_probe
#             )