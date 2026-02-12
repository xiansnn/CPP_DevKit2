#pragma once
#include "t_rtos_blinker_config.h"
#include "sw/widget/rtos_widget.h"
#include "t_rtos_blinker_main_classes.h"

struct struct_ConfigClockWidgetElement
{
    int length;
    ColorIndex color{ColorIndex::WHITE};
};

extern std::map<ControlledObjectStatus, std::string> status_to_string;

extern struct_ConfigGraphicWidget clock_widget_config;
extern struct_ConfigClockWidgetElement hour_widget_element_config;
extern struct_ConfigClockWidgetElement minute_widget_element_config;
extern struct_ConfigClockWidgetElement second_widget_element_config;

class ClockWidgetElement
{
protected:
    std::string name;
    ControlledObjectStatus status;

    int angle_degree;
    int length;
    ColorIndex color;

public:
    rtos_GraphicWidget *host_widget;
    ClockWidgetElement(rtos_GraphicWidget *host_widget, struct_ConfigClockWidgetElement element_cfg);
    ~ClockWidgetElement();
};

class HourWidgetElement : public rtos_Widget, public ClockWidgetElement, public rtos_BlinkingWidget
{
private:
    /* data */
public:
    HourWidgetElement(rtos_GraphicWidget *host_widget, rtos_Model *actual_displayed_model, struct_ConfigClockWidgetElement element_cfg);
    ~HourWidgetElement();
    void draw();
    void get_value_of_interest();

    void save_canvas_color() override;
    void restore_canvas_color() override;
    void blink() override;
    void set_focus_color() override;
};

class MinuteWidgetElement : public rtos_Widget, public ClockWidgetElement, public rtos_BlinkingWidget
{
private:
    /* data */
public:
    MinuteWidgetElement(rtos_GraphicWidget *host_widget, rtos_Model *actual_displayed_model, struct_ConfigClockWidgetElement element_cfg);
    ~MinuteWidgetElement();
    void draw();
    void get_value_of_interest();

    void save_canvas_color() override;
    void restore_canvas_color() override;
    void blink() override;
    void set_focus_color() override;
};
class SecondWidgetElement : public rtos_Widget, public ClockWidgetElement, public rtos_BlinkingWidget
{
private:
    /* data */
public:
    SecondWidgetElement(rtos_GraphicWidget *host_widget, rtos_Model *actual_displayed_model, struct_ConfigClockWidgetElement element_cfg);
    ~SecondWidgetElement();
    void draw();
    void get_value_of_interest();

    void save_canvas_color() override;
    void restore_canvas_color() override;
    void blink() override;
    void set_focus_color() override;
};

class ClockWidget : public rtos_GraphicWidget
{
private:
    int hour_length;
    int minute_length;
    uint second_length;
    uint radius;
    ColorIndex face_color{ColorIndex::WHITE};

    uint x_center;
    uint y_center;

    ControlledObjectStatus clock_status;

    void draw_dial();

public:
    void draw_clock_hands(int angle_degree, uint length, ColorIndex color);
    int hour_angle_degree;
    uint minute_angle_degree;
    uint second_angle_degree;
    ColorIndex hour_color;
    ColorIndex minute_color;
    ColorIndex second_color;
    ClockWidget(rtos_Model *actual_displayed_model,
                struct_ConfigGraphicWidget graph_cfg,
                CanvasFormat canvas_format,
                rtos_DisplayDevice *display_device);
    ~ClockWidget();

    void draw() override;
    void get_value_of_interest() override;
};
