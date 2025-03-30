add_library(device_st7735 st7735.cpp  )
target_link_libraries(device_st7735 
            pico_stdlib 
            hw_spi 
            sw_display_device 
            )