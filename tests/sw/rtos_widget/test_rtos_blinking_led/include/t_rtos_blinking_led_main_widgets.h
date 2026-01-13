#pragma once
#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"
#include "t_rtos_blinking_led_main_classes.h"


extern struct_ConfigTextWidget clock_hour_text_cfg;
extern struct_ConfigTextWidget clock_minute_text_cfg;
extern struct_ConfigTextWidget clock_second_text_cfg;

class my_hour_text_widget : public rtos_TextWidget
{
private:
    ControlledObjectStatus hour_status;
    int hour_value;
public:
    my_hour_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                        struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model);
    ~my_hour_text_widget();
    void get_value_of_interest();
    void draw();
};
class my_minute_text_widget : public rtos_TextWidget
{
private:
    ControlledObjectStatus minute_status;
    int minute_value;
public:
    my_minute_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                        struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model);
    ~my_minute_text_widget();
    void get_value_of_interest();
    void draw();
};
class my_second_text_widget : public rtos_TextWidget
{
private:
    ControlledObjectStatus second_status;
    int second_value;
public:
    my_second_text_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                        struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model);
    ~my_second_text_widget();
    void get_value_of_interest();
    void draw();
};
