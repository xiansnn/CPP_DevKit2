/**
 * @file t_managed_square_led_widgets.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-12
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "sw/ui_core/ui_core.h"
#include "sw/widget_square_led/widget_square_led.h"
#include "t_managed_square_led_models.cpp"
#include "utilities/probe/probe.h"


Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

class my_simple_led_widget : public WidgetSquareLed
{
private:
public:
    my_simple_led_widget(MyManagedSquareLedModel *actual_displayed_model,
                         GraphicDisplayDevice *graphic_display_screen,
                         struct_ConfigGraphicWidget graph_cfg);
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
                               struct_ConfigGraphicWidget graph_cfg);
    ~MySquareLEDWidgetWithFocus();
    void draw();
    void get_value_of_interest();
};

my_simple_led_widget::my_simple_led_widget(MyManagedSquareLedModel *actual_displayed_model,
                                           GraphicDisplayDevice *graphic_display_screen,
                                           struct_ConfigGraphicWidget graph_cfg)
    : WidgetSquareLed(actual_displayed_model, graphic_display_screen, graph_cfg)
{
}

my_simple_led_widget::~my_simple_led_widget()
{
}

void my_simple_led_widget::get_value_of_interest() // le valeu bool
{
    this->led_is_on = ((MyManagedSquareLedModel *)this->actual_displayed_model)->my_bool_value;
}

MySquareLEDWidgetWithFocus::MySquareLEDWidgetWithFocus(MyManagedSquareLedModel *actual_displayed_model,
                                                       GraphicDisplayDevice *display_screen,
                                                       struct_ConfigGraphicWidget graph_cfg)
    : GraphicWidget(display_screen, graph_cfg, actual_displayed_model)
{
#define FOCUS_OFFSET 8u
#define FOCUS_WIDTH 5u
    uint8_t anchor_x = graph_cfg.widget_anchor_x + FOCUS_OFFSET;

    struct_ConfigGraphicWidget square_led_cfg{
        .pixel_frame_width = graph_cfg.pixel_frame_width - FOCUS_OFFSET,
        .pixel_frame_height = graph_cfg.pixel_frame_height,
        .fg_color = graph_cfg.fg_color,
        .bg_color = graph_cfg.bg_color,
        .widget_anchor_x = anchor_x,
        .widget_anchor_y = graph_cfg.widget_anchor_y,
        .widget_with_border = true};
    this->square_led = new my_simple_led_widget(actual_displayed_model, display_screen, square_led_cfg);

    struct_ConfigGraphicWidget focus_led_cfg{
        .pixel_frame_width = FOCUS_WIDTH,
        .pixel_frame_height = graph_cfg.pixel_frame_height,
        .fg_color = graph_cfg.fg_color,
        .bg_color = graph_cfg.bg_color,
        .widget_anchor_x = FOCUS_OFFSET,
        .widget_anchor_y = graph_cfg.widget_anchor_y,
        .widget_with_border = false};

    this->focus_led = new WidgetFocusIndicator(actual_displayed_model,
                                              display_screen,
                                              focus_led_cfg);
    this->focus_led->set_blink_us(200000);
    add_widget(this->square_led);
    add_widget(this->focus_led);

    // this->actual_displayed_model->set_change_flag(); // otherwise nothing is drawn before we act on the rotary encoder
}

MySquareLEDWidgetWithFocus::~MySquareLEDWidgetWithFocus()
{
}

void MySquareLEDWidgetWithFocus::draw()
{
    get_value_of_interest(); 
    if (actual_displayed_model->has_changed())
    //nothing to draw
        actual_displayed_model->draw_widget_done();

    for (auto &&widget : widgets)
    {
        pr_D4.hi();
        widget->draw();
        pr_D4.lo();
    }
}

void MySquareLEDWidgetWithFocus::get_value_of_interest()
{
    //nothing to get
}
