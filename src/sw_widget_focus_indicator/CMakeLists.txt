add_library(widget_focus_indicator widget_focus_indicator.cpp)
target_link_libraries(widget_focus_indicator
            ui_core
            widget_square_led
            )