/**
 * @file t_managed_horizontal_bar_widgets.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "ui_core.h"
#include "widget_horizontal_bar.h"
#include "widget_square_led.h"
#include "t_managed_horizontal_bar_models.cpp"
#include "probe.h"

Probe pr_D5 = Probe(5);

class My_Horizontal_Bar_Widget : public WidgetHorizontalBar
{
private:
public:
    My_Horizontal_Bar_Widget(Model *bar_value_model,
                             GraphicDisplayDevice *graphic_display_screen,
                             int max_value, int min_value,
                             struct_ConfigGraphicWidget graph_cfg);
    ~My_Horizontal_Bar_Widget();
    void get_value_of_interest();
};

My_Horizontal_Bar_Widget::My_Horizontal_Bar_Widget(Model *bar_value_model,
                                                   GraphicDisplayDevice *graphic_display_screen,
                                                   int max_value, int min_value,
                                                   struct_ConfigGraphicWidget graph_cfg)
    : WidgetHorizontalBar(bar_value_model, graphic_display_screen,
                          max_value, min_value, graph_cfg)
{
}

My_Horizontal_Bar_Widget::~My_Horizontal_Bar_Widget()
{
}

void My_Horizontal_Bar_Widget::get_value_of_interest()
{
    this->set_level(((UIControlledIncrementalValue *)this->actual_displayed_model)->get_value());
}

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
                                        struct_ConfigGraphicWidget graph_cfg);
    ~My_Horizontal_Bar_Widget_With_Focus();
    void draw();
    void get_value_of_interest();
};

My_Horizontal_Bar_Widget_With_Focus::My_Horizontal_Bar_Widget_With_Focus(MyControlledHorizontalBarModel *bar_value_model,
                                                                         GraphicDisplayDevice *graphic_display_screen,
                                                                         int max_value, int min_value,
                                                                         struct_ConfigGraphicWidget graph_cfg)
    : GraphicWidget(graphic_display_screen, graph_cfg, bar_value_model)

{
    this->bar_value_model = bar_value_model;

#define FOCUS_OFFSET 8
#define FOCUS_WIDTH 5

    uint8_t anchor_x = widget_anchor_x + FOCUS_OFFSET;

    struct_ConfigGraphicWidget h_bar_cfg{
        .pixel_frame_width = graph_cfg.pixel_frame_width - FOCUS_OFFSET,
        .pixel_frame_height = graph_cfg.pixel_frame_height,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK,
        .widget_anchor_x = anchor_x,
        .widget_anchor_y = widget_anchor_y,
        .widget_with_border = true};

    this->h_bar_widget = new My_Horizontal_Bar_Widget(bar_value_model,
                                                      graphic_display_screen,
                                                      max_value, min_value,
                                                      h_bar_cfg);

    struct_ConfigGraphicWidget focus_cfg{
        .pixel_frame_width = FOCUS_WIDTH,
        .pixel_frame_height = graph_cfg.pixel_frame_height,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK,
        .widget_anchor_x = widget_anchor_x,
        .widget_anchor_y = widget_anchor_y,
        .widget_with_border = false};

    this->focus_widget = new WidgetFocusIndicator(bar_value_model,
                                                  graphic_display_screen,
                                                  focus_cfg);
    this->focus_widget->set_blink_us(200000);

    this->add_widget(focus_widget);
    this->add_widget(h_bar_widget);

    this->actual_displayed_model->set_change_flag();
}

My_Horizontal_Bar_Widget_With_Focus::~My_Horizontal_Bar_Widget_With_Focus()
{
}

void My_Horizontal_Bar_Widget_With_Focus::draw()
{

    get_value_of_interest();
    if (actual_displayed_model->has_changed())
    {
        actual_displayed_model->draw_widget_done();
    }
    for (auto &&widget : widgets)
    {
        widget->draw();
    }

}

void My_Horizontal_Bar_Widget_With_Focus::get_value_of_interest()
{
}
