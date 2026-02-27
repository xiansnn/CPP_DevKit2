#pragma once
#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"
#include "t_rtos_blinker_main_classes.h"
#include "t_rtos_blinker_config.h"

extern struct_ConfigTextWidget clock_hour_text_cfg;
extern struct_ConfigTextWidget clock_minute_text_cfg;
extern struct_ConfigTextWidget clock_second_text_cfg;

class my_digital_clock_widget : public rtos_TextWidget, public rtos_BlinkingWidget
{
private:
    ControlledObjectStatus digital_clock_status;
    int digital_clock_value;
    ClockElementType clock_element_type;
public:
    my_digital_clock_widget(ClockElementType clock_element_type, rtos_GraphicDisplayDevice *graphic_display_screen, rtos_Blinker *blinker,
                            struct_ConfigTextWidget text_cfg, CanvasFormat format, rtos_Model *model);
    ~my_digital_clock_widget();
    void get_value_of_interest();
    void draw();
    void blink();
    void save_canvas_color();
    void restore_canvas_color();
    void set_focus_color();
};