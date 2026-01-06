#pragma once
#include "t_rtos_blinking_led_config.h"
#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"

class focus_dummy_widget : public rtos_Widget
{
private:
    int current_focus_index;
    std::string status_string;

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
    std::string status_string;
    int hour_value;
    int minute_value;
    int second_value;

public:
    clock_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device);
    ~clock_dummy_widget();
    void draw();
    void get_value_of_interest();
};

class hour_dummy_widget : public rtos_Widget
{
private:
    std::string status_string;
    int hour_value;

public:
    hour_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device);
    ~hour_dummy_widget();
    void draw();
    void get_value_of_interest();
};
class minute_dummy_widget : public rtos_Widget
{
private:
    std::string status_string;
    int minute_value;

public:
    minute_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device);
    ~minute_dummy_widget();
    void draw();
    void get_value_of_interest();
};
class second_dummy_widget : public rtos_Widget
{
private:
    std::string status_string;
    int second_value;

public:
    second_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device);
    ~second_dummy_widget();
    void draw();
    void get_value_of_interest();
};
