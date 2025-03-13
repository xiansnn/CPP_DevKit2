add_executable(test_pwm test_pwm.cpp  )
target_link_libraries(test_pwm
            pico_stdlib 
            hw_pwm
            )
#-------------------------------------------------------------------------------------
add_executable(test_pwm_duty  test_pwm_duty.cpp   )
target_link_libraries(test_pwm_duty
            pico_stdlib
            hw_pwm
            utilities_probe
            )
#-------------------------------------------------------------------------------------
add_executable(test_pwm_irq  test_pwm_irq.cpp )
target_link_libraries(test_pwm_irq
            pico_stdlib
            hw_pwm
            utilities_probe
            )