#pragma once

#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"

extern struct_ConfigTextWidget clock_monitoring_text_cfg;
extern struct_ConfigTextWidget controller_monitoring_text_cfg;
extern rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper;

class my_controller_monitoring_widget : public rtos_TextWidget
{
private:
    std::string controller_status;
    int focus_index;
    std::string focus_name;

public:
    my_controller_monitoring_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                         struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model);
    ~my_controller_monitoring_widget();
    void get_value_of_interest();
    void draw();
};


class my_clock_monitoring_widget : public rtos_TextWidget
{
private:
    std::string clock_status;
    std::string hour_status;
    std::string minute_status;
    std::string second_status;
    std::string hand_name;
    int hh;
    int mm;
    int ss;

public:
    my_clock_monitoring_widget(rtos_GraphicDisplayDevice *graphic_display_screen,
                         struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model);
    ~my_clock_monitoring_widget();
    void get_value_of_interest();
    void draw();
};
