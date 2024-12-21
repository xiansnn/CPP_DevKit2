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
# add_executable(test_switch_button test_switch_button_irq.cpp)
# pico_enable_stdio_uart(test_switch_button 1)
# pico_enable_stdio_usb(test_switch_button 0)
# # pico_set_program_name(test_switch_button "test_switch_button")
# # pico_set_program_version(test_switch_button "0.1")
# target_link_libraries(test_switch_button  
#     pico_stdlib
#     switch_button
#     debug_probe
#     )
# pico_add_extra_outputs(test_switch_button)

#-------------------------------------------------------------------------------------

