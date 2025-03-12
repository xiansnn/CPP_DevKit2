add_library(device_ky040 ky040.cpp)
target_link_libraries(device_ky040 
            pico_stdlib 
            device_rotary_encoder
            sw_ui_core
             )