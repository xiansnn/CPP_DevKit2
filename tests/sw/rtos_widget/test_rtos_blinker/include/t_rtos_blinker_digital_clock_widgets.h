#pragma once
#include "sw/widget/rtos_widget.h"
#include "sw/ui_core/rtos_ui_core.h"
#include "t_rtos_blinker_main_classes.h"
#include "t_rtos_blinker_config.h"

class DigitalClockWidgetElement : public rtos_TextWidget, public rtos_BlinkingWidget
{
private:
    ControlledObjectStatus digital_clock_status;
    int digital_clock_value;
    ClockElementType clock_element_type;

public:
    DigitalClockWidgetElement(ClockElementType clock_element_type,
                              rtos_GraphicDisplayDevice *graphic_display_screen,
                              rtos_Blinker *blinker,
                              struct_ConfigTextWidget text_cfg,
                              CanvasFormat format, rtos_Model *model);
    ~DigitalClockWidgetElement();
    void get_value_of_interest();
    void draw();
    void blink();
    void save_canvas_color();
    void restore_canvas_color();
    void set_focus_color();
};

class DigitalClockWidget : public rtos_Widget
{
private:
    /* data */
public:
    DigitalClockWidgetElement *clock_hour_widget_element;
    DigitalClockWidgetElement *clock_minute_widget_element;
    DigitalClockWidgetElement *clock_second_widget_element;

    DigitalClockWidget(rtos_Model *model,
                       rtos_Blinker *blinker,
                       CanvasFormat format,
                       rtos_GraphicDisplayDevice *graphic_display_screen);
    ~DigitalClockWidget();
    void get_value_of_interest();
    void draw();
};
