add_library(widget_text widget_text.cpp)
target_link_libraries(widget_text
            ui_core
            framebuffer
            widget
            )