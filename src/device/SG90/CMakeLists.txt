add_library(device_sg90 sg90.cpp)
target_link_libraries(device_sg90  
                pico_stdlib 
                hw_pwm
                )