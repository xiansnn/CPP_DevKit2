add_executable(test_rtos_st7735_graphic_primitives  test_rtos_st7735_graphic_primitives.cpp  )
target_link_libraries(test_rtos_st7735_graphic_primitives PUBLIC
            pico_stdlib 
            rtos_hw_spi
            device_st7735 
            rtos_widget
            utilities_probe
            
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_st7735_text_and_graph_widgets 
                test_rtos_st7735_text_and_graph_widgets.cpp
                t_rtos_common_text_and_graph_widgets.cpp
                )
target_link_libraries(test_rtos_st7735_text_and_graph_widgets 
            pico_stdlib 
            utilities_probe
            rtos_ui_core
            rtos_hw_spi
            rtos_widget
            device_st7735 
            sw_display_device
            
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_st7735_text_primitives test_rtos_st7735_text_primitives.cpp)
target_link_libraries(test_rtos_st7735_text_primitives 
            pico_stdlib 
            rtos_hw_spi
            device_st7735 
            rtos_widget
            utilities_probe
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_ssd1306_graphic_primitives  test_rtos_ssd1306_graphic_primitives.cpp  )
target_link_libraries(test_rtos_ssd1306_graphic_primitives 
            pico_stdlib 
            rtos_hw_i2c 
            device_ssd1306 
            # sw_widget
            rtos_widget
            # sw_ui_core
            sw_display_device
            utilities_probe
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_ssd1306_text_primitives test_rtos_ssd1306_text_primitives.cpp)
target_link_libraries(test_rtos_ssd1306_text_primitives 
            pico_stdlib 
            rtos_hw_i2c 
            rtos_widget
            # sw_ui_core
            device_ssd1306 
            utilities_probe
            FreeRTOS-Kernel-Heap4 
            )
#-------------------------------------------------------------------------------------
add_executable(test_rtos_ssd1306_text_and_graph_widgets 
                test_rtos_ssd1306_text_and_graph_widgets.cpp
                t_rtos_common_text_and_graph_widgets.cpp
                )
target_link_libraries(test_rtos_ssd1306_text_and_graph_widgets 
            pico_stdlib 
            utilities_probe
            rtos_ui_core
            rtos_hw_i2c
            rtos_widget
            device_ssd1306
            sw_display_device
            FreeRTOS-Kernel-Heap4  
            )

add_subdirectory(test_rtos_widget_extended)
#-------------------------------------------------------------------------------------
# add_executable(test_rtos_all_device_text_and_graph_widgets 
#                 test_rtos_all_device_text_and_graph_widgets.cpp
#                 t_rtos_common_text_and_graph_widgets.cpp
#                 t_rtos_all_device_roll_control.cpp
#                 )
# target_link_libraries(test_rtos_all_device_text_and_graph_widgets 
#             pico_stdlib 
#             utilities_probe
#             rtos_ui_core
#             rtos_hw_spi
#             rtos_hw_i2c
#             rtos_widget
#             device_ssd1306
#             device_st7735 
#             dev_rtos_rotary_encoder
#             sw_display_device
#             FreeRTOS-Kernel-Heap4
#             )
           
