add_library(sw_widget   
            widget.cpp
            canvas.cpp)
target_link_libraries(sw_widget 
            pico_stdlib
            sw_ui_core
            FreeRTOS-Kernel-Heap4
            )
            
add_library(rtos_widget   
            rtos_widget.cpp
            widget_primitives.cpp)
target_link_libraries(rtos_widget 
            pico_stdlib
            rtos_ui_core
            FreeRTOS-Kernel-Heap4
            )

