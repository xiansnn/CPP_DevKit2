add_executable(test_horizontal_bar test_horizontal_bar.cpp
                                test_horizontal_bar_widgets.cpp
                                test_horizontal_bar_models.cpp)

include_directories(test_horizontal_bar PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include  )
target_link_libraries(test_horizontal_bar
            hw_i2c
            device_ssd1306
            sw_ui_core
            utilities_probe
            sw_widget_horizontal_bar
            )    
  