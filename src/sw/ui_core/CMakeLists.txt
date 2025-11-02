add_library(sw_ui_core ui_core.cpp)
target_link_libraries(sw_ui_core 
            pico_stdlib 
            )

add_library(rtos_ui_core rtos_ui_core.cpp)
target_link_libraries(rtos_ui_core 
            pico_stdlib 
            sw_ui_core
            sw_widget
            sw_display_device
            FreeRTOS-Kernel-Heap4
            )
