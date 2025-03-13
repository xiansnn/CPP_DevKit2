add_executable(test_hc_sr04 test_hc_sr04.cpp )
target_link_libraries(test_hc_sr04 
            pico_stdlib 
            device_hc_sr04
            )