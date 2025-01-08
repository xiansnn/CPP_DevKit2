add_library(ky040 ky040.cpp)
target_link_libraries(ky040 
                        pico_stdlib 
                        rotary_encoder
                        ui_core
                         )