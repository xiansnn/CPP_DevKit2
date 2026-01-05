#pragma once
#include "t_rtos_blinking_led_config.h"
#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"

// extern struct_ConfigTextWidget hour_text_cfg;
// extern struct_ConfigTextWidget minute_text_cfg;
// extern struct_ConfigTextWidget second_text_cfg;
// extern struct_ConfigTextWidget focus_dummy_text_cfg;

class focus_dummy_widget : public rtos_Widget
{
private:
    int current_focus_index;
    std::string status;

public:
    focus_dummy_widget(rtos_Model *actual_displayed_model,
                       rtos_DisplayDevice *display_device);
    ~focus_dummy_widget();
    void draw();
    void get_value_of_interest();
};

class clock_dummy_widget : public rtos_Widget
{
private:
    std::string status;
    int hour;
    int minute;
    int second;

public:
    clock_dummy_widget(rtos_Model *actual_displayed_model,rtos_DisplayDevice *display_device);
    ~clock_dummy_widget();
    void draw();
    void get_value_of_interest();
};
