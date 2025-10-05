add_library(device_st7735 st7735.cpp  )
target_link_libraries(device_st7735 PUBLIC
            pico_stdlib 
            rtos_hw_spi 
            sw_display_device 
            )