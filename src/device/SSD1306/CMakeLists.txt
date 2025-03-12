add_library(device_ssd1306 ssd1306.cpp  )
target_link_libraries(device_ssd1306 
            pico_stdlib 
            hw_i2c 
            sw_display_device 
            )
