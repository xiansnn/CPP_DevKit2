#-------------------------------------------------------------------------------------
add_executable(test_debug_probe test_debug_probe.cpp)
pico_enable_stdio_uart(test_debug_probe 1)
pico_enable_stdio_usb(test_debug_probe 0)
# pico_set_program_name(test_debug_probe "test_debug_probe")
# pico_set_program_version(test_debug_probe "0.1")
target_link_libraries(test_debug_probe
    pico_stdlib 
    debug_probe
    )
# pico_add_extra_outputs(test_debug_probe)
#-------------------------------------------------------------------------------------
add_executable(test_switch_button test_switch_button_irq.cpp)
pico_enable_stdio_uart(test_switch_button 1)
pico_enable_stdio_usb(test_switch_button 0)
# pico_set_program_name(test_switch_button "test_switch_button")
# pico_set_program_version(test_switch_button "0.1")
target_link_libraries(test_switch_button  
    pico_stdlib
    switch_button
    debug_probe
    )
# pico_add_extra_outputs(test_switch_button)

#-------------------------------------------------------------------------------------
add_executable(test_rotary_encoder test_rotary_encoder.cpp)
pico_enable_stdio_uart(test_rotary_encoder 1)
pico_enable_stdio_usb(test_rotary_encoder 0)
# pico_set_program_name(test_rotary_encoder "test_rotary_encoder")
# pico_set_program_version(test_rotary_encoder "0.1")
target_link_libraries(test_rotary_encoder  
                        pico_stdlib  
                        rotary_encoder 
                        debug_probe 
                        )
# pico_add_extra_outputs(test_rotary_encoder)

#-------------------------------------------------------------------------------------
add_executable(test_i2c_bus_scan test_i2c_bus_scan.cpp)
target_link_libraries(test_i2c_bus_scan 
                        hw_i2c 
                        debug_probe
                        )
# pico_add_extra_outputs(test_i2c_bus_scan)
#-------------------------------------------------------------------------------------
add_executable(test_i2c_xchange test_i2c_xchange.cpp)
target_link_libraries(test_i2c_xchange 
                        hw_i2c 
                        debug_probe
                        )
# # pico_add_extra_outputs(test_i2c_xchange)
#-------------------------------------------------------------------------------------
add_executable(test_dht11  test_dht.cpp  )   
target_link_libraries(test_dht11 
            pico_stdlib 
            dht11
            debug_probe
            )
# pico_add_extra_outputs(test_dht11)
#-------------------------------------------------------------------------------------

