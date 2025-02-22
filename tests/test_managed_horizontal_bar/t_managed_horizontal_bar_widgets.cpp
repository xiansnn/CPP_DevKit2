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
#include "probe.h"

Probe pr_D5 = Probe(5);

class MyHorizontalBarWidgetWithFocus : public GraphicWidget
{
private:
    WidgetHorizontalBar *h_bar_widget;
    WidgetFocusIndicator *focus_widget;
    MyHorizontalBarModel *bar_value_model;

public:
    MyHorizontalBarWidgetWithFocus(MyHorizontalBarModel *bar_value_model,
                                   GraphicDisplayDevice *graphic_display_screen,
                                   int max_value, int min_value,
                                   struct_ConfigGraphicFramebuffer graph_cfg,
                                   uint8_t widget_anchor_x, uint8_t widget_anchor_y);
    ~MyHorizontalBarWidgetWithFocus();
    void draw();
    void blink_refresh();
};

MyHorizontalBarWidgetWithFocus::MyHorizontalBarWidgetWithFocus(MyHorizontalBarModel *bar_value_model,
                                                               GraphicDisplayDevice *graphic_display_screen,
                                                               int max_value, int min_value,
                                                               struct_ConfigGraphicFramebuffer graph_cfg,
                                                               uint8_t widget_anchor_x, uint8_t widget_anchor_y)
    : GraphicWidget(graphic_display_screen, bar_value_model, graph_cfg, widget_anchor_x, widget_anchor_y, false)

{
    this->bar_value_model = bar_value_model;

#define FOCUS_OFFSET 8
#define FOCUS_WIDTH 5

    struct_ConfigGraphicFramebuffer h_bar_cfg{
        .frame_width = graph_cfg.frame_width - FOCUS_OFFSET,
        .frame_height = graph_cfg.frame_height,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK};

    this->h_bar_widget = new WidgetHorizontalBar(bar_value_model,
                                                 graphic_display_screen,
                                                 max_value, min_value,
                                                 h_bar_cfg,
                                                 widget_anchor_x + FOCUS_OFFSET, widget_anchor_y);

    struct_ConfigGraphicFramebuffer focus_cfg{
        .frame_width = FOCUS_WIDTH,
        .frame_height = graph_cfg.frame_height,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK};

    this->focus_widget = new WidgetFocusIndicator(bar_value_model,
                                                  graphic_display_screen,
                                                  focus_cfg,
                                                  widget_anchor_x, widget_anchor_y, false);
    this->focus_widget->set_blink_us(200000);

    this->add_widget(focus_widget);
    this->add_widget(h_bar_widget);

    this->actual_displayed_model->set_change_flag();
}

MyHorizontalBarWidgetWithFocus::~MyHorizontalBarWidgetWithFocus()
{
}

void MyHorizontalBarWidgetWithFocus::draw()
{
}

void MyHorizontalBarWidgetWithFocus::blink_refresh()
{
    this->focus_widget->blink_refresh();
}

