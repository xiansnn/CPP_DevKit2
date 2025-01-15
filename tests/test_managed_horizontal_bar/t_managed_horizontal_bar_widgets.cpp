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
#include "widget_focus_indicator.h"

class MyHorizontalBarWidgetWithFocus : public Widget
{
private:
    WidgetHorizontalBar *h_bar_widget;
    WidgetFocusIndicator *focus_widget;
    MyHorizontalBarModel *bar_value_model;

public:
    MyHorizontalBarWidgetWithFocus(MyHorizontalBarModel *bar_value_model,
                                   DisplayDevice *display_screen,
                                   int max_value, int min_value,
                                   size_t frame_width, size_t frame_height,
                                   uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                   bool widget_with_border = true,
                                   uint8_t widget_border_width = 1,
                                   FramebufferColor color = FramebufferColor::WHITE,
                                   FramebufferFormat framebuffer_format = FramebufferFormat::MONO_VLSB);
    ~MyHorizontalBarWidgetWithFocus();
    void draw_refresh();
};

MyHorizontalBarWidgetWithFocus::MyHorizontalBarWidgetWithFocus(MyHorizontalBarModel *bar_value_model,
                                                               DisplayDevice *display_screen,
                                                               int max_value, int min_value,
                                                               size_t frame_width, size_t frame_height,
                                                               uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                                               bool widget_with_border,
                                                               uint8_t widget_border_width,
                                                               FramebufferColor color,
                                                               FramebufferFormat framebuffer_format)
    : Widget(display_screen, frame_width, frame_height, widget_anchor_x, widget_anchor_y, false)

{
    this->bar_value_model = bar_value_model;
    this->h_bar_widget = new WidgetHorizontalBar(bar_value_model,
                                                 display_screen,
                                                 max_value, min_value,
                                                 frame_width - 8, frame_height,
                                                 widget_anchor_x + 8, widget_anchor_y,
                                                 widget_with_border,
                                                 widget_border_width,
                                                 color,
                                                 framebuffer_format);
    this->focus_widget = new WidgetFocusIndicator(bar_value_model, display_screen,
                                                  5, frame_height,
                                                  widget_anchor_x, widget_anchor_y, false);
    this->focus_widget->set_blink_us(200000);

    this->add_widget(h_bar_widget);
    this->add_widget(focus_widget);
}

MyHorizontalBarWidgetWithFocus::~MyHorizontalBarWidgetWithFocus()
{
}

void MyHorizontalBarWidgetWithFocus::draw_refresh()
{
    this->h_bar_widget->set_value(this->bar_value_model->get_value());
    for (auto &&w : this->widgets)
    {
        w->draw_refresh();
    }
    this->bar_value_model->clear_change_flag();
}