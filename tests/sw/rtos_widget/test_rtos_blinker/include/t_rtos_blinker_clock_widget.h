#pragma once
#include "t_rtos_blinker_config.h"
#include "sw/widget/rtos_widget.h"

extern struct_ConfigGraphicWidget clock_widget_config;
extern std::map<ControlledObjectStatus, std::string> status_to_string;

class WidgetElement : public rtos_Widget, public rtos_BlinkingWidget
{
protected:
    std::string name;
    ControlledObjectStatus status;

public:
    rtos_GraphicWidget *host_widget;
    WidgetElement(rtos_GraphicWidget *host_widget,
                  rtos_Model *actual_displayed_model,
                  rtos_DisplayDevice *display_device = nullptr);
    ~WidgetElement();

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
    int hour_angle_degree;
    int hour_length;
    int minute_length;
    uint second_length;
    uint minute_angle_degree;
    uint second_angle_degree;
    uint radius;
    ColorIndex face_color{ColorIndex::WHITE};

    uint x_center;
    uint y_center;

    ControlledObjectStatus clock_status;

    void draw_dial(uint number_of_divisions, uint number_of_subdivisions);
    void draw_clock_hands(int angle_degree, uint length, ColorIndex color);

public:
    ColorIndex hour_color{ColorIndex::YELLOW};
    ColorIndex minute_color{ColorIndex::YELLOW};
    ColorIndex second_color{ColorIndex::RED};
    ClockWidget(rtos_Model *actual_displayed_model,
                struct_ConfigGraphicWidget graph_cfg,
                CanvasFormat canvas_format,
                rtos_DisplayDevice *display_device);
    ~ClockWidget();

    void draw() override;
    void get_value_of_interest() override;
};
