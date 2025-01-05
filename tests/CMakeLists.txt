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
add_executable(test_pwm test_pwm.cpp  )
target_link_libraries(test_pwm
            pico_stdlib 
            hw_pwm
            )
# pico_add_extra_outputs(test_pwm)
#-------------------------------------------------------------------------------------
add_executable(test_pwm_duty  test_pwm_duty.cpp   )
target_link_libraries(test_pwm_duty
            pico_stdlib
            debug_probe
            hw_pwm
            )
# pico_add_extra_outputs(test_pwm_duty)
#-------------------------------------------------------------------------------------
add_executable(test_pwm_irq  test_pwm_irq.cpp )
target_link_libraries(test_pwm_irq
            pico_stdlib
            debug_probe
            hw_pwm
            )
# pico_add_extra_outputs(test_pwm_irq)
#-------------------------------------------------------------------------------------
add_executable(test_sg90 test_SG90.cpp)
target_link_libraries(test_sg90
            sg90
            )
# pico_add_extra_outputs(test_sg90)
#-------------------------------------------------------------------------------------
add_executable(test_hc_sr04 test_hc_sr04.cpp )
target_link_libraries(test_hc_sr04 
            pico_stdlib 
            hc_sr04
            )
# pico_add_extra_outputs(test_hc_sr04)
#-------------------------------------------------------------------------------------
add_executable(test_mpu6050   test_mpu6050.cpp   )
target_link_libraries(test_mpu6050 
            pico_stdlib 
            mpu6050 
            hw_i2c 
            debug_probe
            )
# pico_add_extra_outputs(test_mpu6050)
#-------------------------------------------------------------------------------------
add_executable(test_mpu6050_irq   test_mpu6050_irq.cpp   )
target_link_libraries(test_mpu6050_irq 
            pico_stdlib 
            mpu6050  
            hw_i2c 
            debug_probe
            )
# pico_add_extra_outputs(test_mpu6050_irq)
#-------------------------------------------------------------------------------------
add_executable(test_ssd1306  test_ssd1306_device.cpp  )
target_link_libraries(test_ssd1306 
            ssd1306 
            pico_stdlib 
            hw_i2c 
            framebuffer
            debug_probe
            )
# pico_add_extra_outputs(test_ssd1306)
#-------------------------------------------------------------------------------------
add_executable(test_framebuffer_streams test_ssd1306_fb_streams.cpp)
target_link_libraries(test_framebuffer_streams 
            pico_stdlib 
            debug_probe
            hw_i2c 
            framebuffer
            ssd1306 
            )
# pico_add_extra_outputs(test_framebuffer_streams)
#-------------------------------------------------------------------------------------
# add_executable(test_msgeq7 test_msgeq7.cpp)
# target_link_libraries(test_msgeq7 
#             pico_stdlib 
#             msgeq7 
#             debug_probe)
# pico_add_extra_outputs(test_msgeq7)
#-------------------------------------------------------------------------------------

