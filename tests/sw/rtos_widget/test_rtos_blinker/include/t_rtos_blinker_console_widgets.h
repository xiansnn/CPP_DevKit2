#pragma once
#include "t_rtos_blinker_config.h"
#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"

class focus_console_widget : public rtos_Widget
{
private:
    int current_focus_index;
    std::string status_string;

public:
    focus_console_widget(rtos_Model *actual_displayed_model,
                       rtos_DisplayDevice *display_device);
    ~focus_console_widget();
    void draw();
    void get_value_of_interest();
};

class clock_console_widget : public rtos_Widget
{
private:
    std::string status_string;
    int hour_value;
    int minute_value;
    int second_value;

public:
    clock_console_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device);
    ~clock_console_widget();
    void draw();
    void get_value_of_interest();
};