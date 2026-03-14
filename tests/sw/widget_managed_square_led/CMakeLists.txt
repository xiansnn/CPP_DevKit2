add_executable(test_managed_square_led test_managed_square_led.cpp
            t_managed_square_led_manager.cpp
            t_managed_square_led_models.cpp
            t_managed_square_led_widgets.cpp
             )
target_include_directories(test_managed_square_led PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(test_managed_square_led 
            pico_stdlib
            sw_widget_square_led
            hw_i2c
            sw_ui_core
            utilities_probe
            device_ssd1306
            device_ky040
            )