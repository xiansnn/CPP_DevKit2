#pragma once

#include "sw/widget_square_led/widget_square_led.h"
#include "t_managed_square_led_models.h"

#define CANVAS_FORMAT CanvasFormat::MONO_VLSB


class my_simple_led_widget : public WidgetSquareLed
{
private:
public:
    my_simple_led_widget(MyManagedSquareLedModel *actual_displayed_model,
                         GraphicDisplayDevice *graphic_display_screen,
                         struct_ConfigGraphicWidget graph_cfg,
                         CanvasFormat format);
    ~my_simple_led_widget();
    void get_value_of_interest();
};

class MySquareLEDWidgetWithFocus : public GraphicWidget
{
private:
    my_simple_led_widget *square_led;
    WidgetFocusIndicator *focus_led;

public:
    MySquareLEDWidgetWithFocus(MyManagedSquareLedModel *actual_displayed_model,
                               GraphicDisplayDevice *display_screen,
                               struct_ConfigGraphicWidget graph_cfg,
                               CanvasFormat format);
    ~MySquareLEDWidgetWithFocus();
    void draw();
    void get_value_of_interest();
};