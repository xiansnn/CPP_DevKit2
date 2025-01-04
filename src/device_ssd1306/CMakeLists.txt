add_library(ssd1306 ssd1306.cpp  )
target_link_libraries(ssd1306 
            pico_stdlib 
            hw_i2c 
            framebuffer 
            )
