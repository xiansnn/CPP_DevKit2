#pragma once
#include "t_rtos_blinker_config.h"
#include "sw/widget/rtos_widget.h"

extern struct_ConfigGraphicWidget clock_widget_config;

enum class CurrentControlledHand
{
    none,
    hour,
    minute,
    second
};

class ClockWidget : public rtos_GraphicWidget,
    public rtos_BlinkingWidget
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
    ColorIndex hour_color{ColorIndex::YELLOW};
    ColorIndex minute_color{ColorIndex::YELLOW};
    ColorIndex second_color{ColorIndex::RED};
    ColorIndex hour_color_bckup;
    ColorIndex minute_color_bckup;
    ColorIndex second_color_bckup;
    uint x_center;
    uint y_center;

    CurrentControlledHand current_controlled_hand;

    ControlledObjectStatus clock_status;

    void draw_dial(uint number_of_divisions, uint number_of_subdivisions);
    void draw_clock_hands(int angle_degree, uint length, ColorIndex color);

public:
    ClockWidget(rtos_Model *actual_displayed_model,
                struct_ConfigGraphicWidget graph_cfg,
                CanvasFormat canvas_format,
                rtos_DisplayDevice *display_device);
    ~ClockWidget();

    void draw() override;
    void get_value_of_interest() override;
    void save_canvas_color();
    void restore_canvas_color();
    void blink();
    void set_focus_color();
};
