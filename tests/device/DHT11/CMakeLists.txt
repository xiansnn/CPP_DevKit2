add_executable(test_dht11  test_dht11.cpp  )   
target_link_libraries(test_dht11 
            pico_stdlib 
            device_dht11
            utilities_probe
            )