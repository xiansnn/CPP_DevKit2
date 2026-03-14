add_executable(test_managed_horizontal_bar  test_managed_horizontal_bar.cpp
            t_managed_horizontal_bar_manager.cpp
            t_managed_horizontal_bar_models.cpp
            t_managed_horizontal_bar_widgets.cpp
            )
target_include_directories(test_managed_horizontal_bar PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(test_managed_horizontal_bar
            pico_stdlib
            hw_i2c
            device_ssd1306
            sw_ui_core
            utilities_probe
            sw_widget_horizontal_bar
            sw_widget_square_led
            device_ky040
            )    