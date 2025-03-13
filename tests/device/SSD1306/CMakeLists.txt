add_executable(test_ssd1306  test_ssd1306_device.cpp  )
target_link_libraries(test_ssd1306 
            pico_stdlib 
            device_ssd1306 
            hw_i2c 
            sw_display_device
            utilities_probe
            )