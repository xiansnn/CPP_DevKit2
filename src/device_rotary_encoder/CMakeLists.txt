add_library(rotary_encoder rotary_encoder.cpp)
target_link_libraries(rotary_encoder 
                        pico_stdlib 
                        switch_button
                         )