add_library(device_rotary_encoder rotary_encoder.cpp)
target_link_libraries(device_rotary_encoder 
            pico_stdlib 
            device_switch_button
            )