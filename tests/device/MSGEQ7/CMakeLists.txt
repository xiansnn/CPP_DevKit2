add_executable(test_msgeq7 test_msgeq7.cpp)
target_link_libraries(test_msgeq7 
            pico_stdlib 
            device_msgeq7 
            utilities_probe)