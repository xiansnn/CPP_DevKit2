#pragma once
#include "t_rtos_blinker_config.h"
#include "sw/widget/rtos_widget.h"

extern struct_ConfigGraphicWidget clock_widget_config;

class ClockWidget : public rtos_GraphicWidget
{
private:
    int hour_angle;
    int hour_length;
    int minute_length;
    uint second_length;
    uint minute_angle;
    uint second_angle;
    uint radius;
    ColorIndex face_color{ColorIndex::WHITE};
    ColorIndex hour_color{ColorIndex::YELLOW};
    ColorIndex minute_color{ColorIndex::YELLOW};
    ColorIndex second_color{ColorIndex::RED};
    uint x_center;
    uint y_center;

    void draw_dial(uint number_of_divisions, uint number_of_subdivisions);
    void draw_clock_hands(int angle, uint length, ColorIndex color);

public:
    ClockWidget(rtos_Model *actual_displayed_model,
                struct_ConfigGraphicWidget graph_cfg,
                CanvasFormat canvas_format,
                rtos_DisplayDevice *display_device);
    ~ClockWidget();

    void draw() override;
    void get_value_of_interest() override;
};
