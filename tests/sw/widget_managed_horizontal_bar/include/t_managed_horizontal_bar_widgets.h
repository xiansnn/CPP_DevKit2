#pragma once

#include "sw/ui_core/ui_core.h"
#include "sw/widget_horizontal_bar/widget_horizontal_bar.h"
#include "sw/widget_square_led/widget_square_led.h"
#include "t_managed_horizontal_bar_models.h"

#define CANVAS_FORMAT CanvasFormat::MONO_VLSB

class My_Horizontal_Bar_Widget : public WidgetHorizontalBar
{
private:
public:
    My_Horizontal_Bar_Widget(Model *bar_value_model,
                             GraphicDisplayDevice *graphic_display_screen,
                             int max_value, int min_value,
                             struct_ConfigGraphicWidget graph_cfg, CanvasFormat format);
    ~My_Horizontal_Bar_Widget();
    void get_value_of_interest();
};

class My_Horizontal_Bar_Widget_With_Focus : public GraphicWidget
{
private:
    My_Horizontal_Bar_Widget *h_bar_widget;
    WidgetFocusIndicator *focus_widget;
    MyControlledHorizontalBarModel *bar_value_model;

public:
    My_Horizontal_Bar_Widget_With_Focus(MyControlledHorizontalBarModel *bar_value_model,
                                        GraphicDisplayDevice *graphic_display_screen,
                                        int max_value, int min_value,
                                        struct_ConfigGraphicWidget graph_cfg, CanvasFormat format);
    ~My_Horizontal_Bar_Widget_With_Focus();
    void draw();
    void get_value_of_interest();
};
