add_library(sg90 sg90.cpp)
target_link_libraries(sg90  
                pico_stdlib 
                hw_pwm
                )