add_executable(test_st7735  test_st7735_device.cpp  )
target_link_libraries(test_st7735 PUBLIC
            pico_stdlib 
            hw_spi
            device_st7735 
            # sw_display_device
            sw_widget
            utilities_probe
            )