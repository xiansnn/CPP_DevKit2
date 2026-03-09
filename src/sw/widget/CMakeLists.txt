add_library(sw_widget   
            widget.cpp
            canvas.cpp
            widget_primitives.cpp)
target_link_libraries(sw_widget 
            pico_stdlib
            sw_ui_core
            )
            
