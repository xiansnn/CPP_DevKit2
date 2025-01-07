add_library(ky040 ky040.cpp)
target_link_libraries(ky040 
                        pico_stdlib 
                        # switch_button
                        rotary_encoder
                        ui_core
                         )