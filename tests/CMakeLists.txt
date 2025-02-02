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
target_link_libraries(test_switch_button  
            pico_stdlib
            switch_button
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_rotary_encoder test_rotary_encoder.cpp)
target_link_libraries(test_rotary_encoder  
            pico_stdlib  
            rotary_encoder 
            debug_probe 
            )
#-------------------------------------------------------------------------------------
add_executable(test_i2c_bus_scan test_i2c_bus_scan.cpp)
target_link_libraries(test_i2c_bus_scan 
            hw_i2c 
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_i2c_xchange test_i2c_xchange.cpp)
target_link_libraries(test_i2c_xchange 
            hw_i2c 
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_dht11  test_dht.cpp  )   
target_link_libraries(test_dht11 
            pico_stdlib 
            dht11
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_pwm test_pwm.cpp  )
target_link_libraries(test_pwm
            pico_stdlib 
            hw_pwm
            )
#-------------------------------------------------------------------------------------
add_executable(test_pwm_duty  test_pwm_duty.cpp   )
target_link_libraries(test_pwm_duty
            pico_stdlib
            debug_probe
            hw_pwm
            )
#-------------------------------------------------------------------------------------
add_executable(test_pwm_irq  test_pwm_irq.cpp )
target_link_libraries(test_pwm_irq
            pico_stdlib
            debug_probe
            hw_pwm
            )
#-------------------------------------------------------------------------------------
add_executable(test_sg90 test_SG90.cpp)
target_link_libraries(test_sg90
            sg90
            )
#-------------------------------------------------------------------------------------
add_executable(test_hc_sr04 test_hc_sr04.cpp )
target_link_libraries(test_hc_sr04 
            pico_stdlib 
            hc_sr04
            )
#-------------------------------------------------------------------------------------
add_executable(test_mpu6050   test_mpu6050.cpp   )
target_link_libraries(test_mpu6050 
            pico_stdlib 
            mpu6050 
            hw_i2c 
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_mpu6050_irq   test_mpu6050_irq.cpp   )
target_link_libraries(test_mpu6050_irq 
            pico_stdlib 
            mpu6050  
            hw_i2c 
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_ssd1306  test_ssd1306_device.cpp  )
target_link_libraries(test_ssd1306 
            ssd1306 
            pico_stdlib 
            hw_i2c 
            framebuffer
            display_device
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_graphic_framebuffer  test_graphic_framebuffer.cpp  )
target_link_libraries(test_graphic_framebuffer 
            ssd1306 
            pico_stdlib 
            hw_i2c 
            framebuffer
            display_device
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_msgeq7 test_msgeq7.cpp)
target_link_libraries(test_msgeq7 
            pico_stdlib 
            msgeq7 
            debug_probe)
#-------------------------------------------------------------------------------------
add_executable(test_ky040 test_ky040.cpp)
target_link_libraries(test_ky040
            pico_stdlib
            ky040
            rotary_encoder
            switch_button
            debug_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_ui_core  test_ui_core.cpp )               
target_link_libraries(test_ui_core 
            pico_stdlib
            ky040
            debug_probe
            widget
            framebuffer
            )
#-------------------------------------------------------------------------------------
add_executable(test_gpio_irq test_gpio_irq.cpp )
target_link_libraries(test_gpio_irq 
            pico_stdlib
            )
#-------------------------------------------------------------------------------------
add_executable(test_square_led test_square_led.cpp )
target_link_libraries(test_square_led 
            pico_stdlib
            widget_square_led
            hw_i2c
            ui_core
            debug_probe
            ssd1306
            switch_button
            )
#-------------------------------------------------------------------------------------
add_executable(test_managed_square_led test_managed_square_led.cpp )
target_link_libraries(test_managed_square_led 
            pico_stdlib
            widget_square_led
            widget_focus_indicator
            hw_i2c
            ui_core
            debug_probe
            ssd1306
            ky040
            )
#-------------------------------------------------------------------------------------
add_executable(test_horizontal_bar test_horizontal_bar.cpp)
target_link_libraries(test_horizontal_bar
            hw_i2c
            ssd1306
            ui_core
            debug_probe
            widget_horizontal_bar
            )    
#-------------------------------------------------------------------------------------
add_executable(test_managed_horizontal_bar test_managed_horizontal_bar.cpp)
target_link_libraries(test_managed_horizontal_bar
            pico_stdlib
            hw_i2c
            ssd1306
            ui_core
            debug_probe
            widget_focus_indicator
            widget_horizontal_bar
            ky040
            )    
#-------------------------------------------------------------------------------------
add_executable(test_horizontal_bargraph test_horizontal_bargraph.cpp)
target_link_libraries(test_horizontal_bargraph
            pico_stdlib
            hw_i2c
            ssd1306
            ui_core
            debug_probe
            widget_bargraph
            widget
            )    
#-------------------------------------------------------------------------------------
add_executable(test_vertical_bargraph test_vertical_bargraph.cpp)
target_link_libraries(test_vertical_bargraph
            pico_stdlib
            hw_i2c
            ssd1306
            ui_core
            debug_probe
            widget_bargraph
            widget
            )    
#-------------------------------------------------------------------------------------
add_executable(test_tuning_dial test_tuning_dial.cpp)
target_link_libraries(test_tuning_dial
            pico_stdlib
            hw_i2c
            ssd1306
            ui_core
            debug_probe
            widget_text
            )    
#-------------------------------------------------------------------------------------
add_executable(test_text_framebuffer test_text_framebuffer.cpp)
target_link_libraries(test_text_framebuffer 
            pico_stdlib 
            debug_probe
            hw_i2c 
            framebuffer
            ssd1306 
            )
#-------------------------------------------------------------------------------------
