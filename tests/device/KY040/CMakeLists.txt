add_executable(test_ky040 test_ky040.cpp)
target_link_libraries(test_ky040
            pico_stdlib
            device_ky040
            device_rotary_encoder
            device_switch_button
            utilities_probe
            )