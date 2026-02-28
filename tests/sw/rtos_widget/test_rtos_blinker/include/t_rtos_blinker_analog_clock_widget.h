#pragma once
#include "t_rtos_blinker_config.h"
#include "sw/widget/rtos_widget.h"
#include "t_rtos_blinker_main_classes.h"

extern std::map<ControlledObjectStatus, std::string> status_to_string;
extern std::map<ClockElementType, std::string> clock_element_to_string;

extern struct_ConfigGraphicWidget analog_clock_widget_config;

class AnalogClockWidgetElement : public rtos_Widget, public rtos_BlinkingWidget
{
protected:
    ControlledObjectStatus status;
    ClockElementType clock_element_type;

    int angle_degree;
    int length;
    ColorIndex fg_element_color;
    ColorIndex bg_element_color;

public:
    rtos_GraphicWidget *host_widget;
    AnalogClockWidgetElement(rtos_GraphicWidget *host_widget, rtos_Blinker *blinker, rtos_Model *actual_displayed_model, ClockElementType clock_element_type);
    ~AnalogClockWidgetElement();

    void draw();
    void get_value_of_interest();

    void save_canvas_color() override;
    void restore_canvas_color() override;
    void blink() override;
    void set_focus_color() override;
};

class AnalogClockWidget : public rtos_GraphicWidget
{
private:
    ControlledObjectStatus clock_status;
    void draw_dial();

public:
    void draw_clock_hands(int angle_degree, uint length, ColorIndex color);

    AnalogClockWidgetElement *clock_hour_widget_element;
    AnalogClockWidgetElement *clock_minute_widget_element;
    AnalogClockWidgetElement *clock_second_widget_element;

    AnalogClockWidget(rtos_Model *actual_displayed_model,
                      rtos_Blinker *blinker,
                      struct_ConfigGraphicWidget graph_cfg,
                      CanvasFormat canvas_format,
                      rtos_DisplayDevice *display_device);
    ~AnalogClockWidget();

    void draw() override;
    void get_value_of_interest() override;
};
