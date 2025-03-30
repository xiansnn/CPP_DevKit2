add_executable(test_st7735  test_st7735_device.cpp  )
target_link_libraries(test_st7735 
            pico_stdlib 
            hw_spi
            device_st7735 
            sw_display_device
            utilities_probe
            )