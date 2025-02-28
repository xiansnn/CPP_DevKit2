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

#include "ui_core.h"
#include "widget_square_led.h"
#include "widget_focus_indicator.h"
#include "t_managed_square_led_models.cpp"
#include "probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

/**
 * @brief MyManagedSquareLedWidget : Example of final implementation of WidgetSquareLed
 */
class MyManagedSquareLedWidget : public WidgetSquareLed
{
private:
public:
    MyManagedSquareLedWidget(MyManagedSquareLedModel *actual_displayed_model,
                             GraphicDisplayDevice *display_screen,
                             struct_ConfigGraphicWidget graph_cfg,
                             uint8_t widget_anchor_x,
                             uint8_t widget_anchor_y);
    ~MyManagedSquareLedWidget();
    void draw();
};
MyManagedSquareLedWidget::MyManagedSquareLedWidget(MyManagedSquareLedModel *actual_displayed_model,
                                                   GraphicDisplayDevice *display_screen,
                                                   struct_ConfigGraphicWidget graph_cfg,
                                                   uint8_t widget_anchor_x,
                                                   uint8_t widget_anchor_y)
    : WidgetSquareLed(actual_displayed_model, display_screen, graph_cfg, widget_anchor_x, widget_anchor_y)
{
    this->led_is_on = true;
}

MyManagedSquareLedWidget::~MyManagedSquareLedWidget()
{
}

void MyManagedSquareLedWidget::draw()
{
    this->led_is_on = ((MyManagedSquareLedModel *)this->actual_displayed_model)->my_bool_value;
    draw_led();
}

class MySquareLEDWidgetWithFocus : public GraphicWidget
{
private:
    MyManagedSquareLedWidget *square_led;
    WidgetFocusIndicator *focus_led;

public:
    MySquareLEDWidgetWithFocus(MyManagedSquareLedModel *actual_displayed_model,
                               GraphicDisplayDevice *display_screen,
                               struct_ConfigGraphicWidget graph_cfg,
                               uint8_t widget_anchor_x,
                               uint8_t widget_anchor_y);
    ~MySquareLEDWidgetWithFocus();
    void draw();
    void draw_refresh();
};

MySquareLEDWidgetWithFocus::MySquareLEDWidgetWithFocus(MyManagedSquareLedModel *actual_displayed_model,
                                                       GraphicDisplayDevice *display_screen,
                                                       struct_ConfigGraphicWidget graph_cfg,
                                                       uint8_t widget_anchor_x,
                                                       uint8_t widget_anchor_y)
    : GraphicWidget(display_screen, actual_displayed_model, graph_cfg, widget_anchor_x, widget_anchor_y, true)
{
#define FOCUS_OFFSET 8
#define FOCUS_WIDTH 5

    struct_ConfigGraphicWidget square_led_cfg{
        .pixel_frame_width = graph_cfg.pixel_frame_width - FOCUS_OFFSET,
        .pixel_frame_height = graph_cfg.pixel_frame_height,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK};

    this->square_led = new MyManagedSquareLedWidget(actual_displayed_model,
                                                    display_screen,
                                                    square_led_cfg,
                                                    widget_anchor_x + FOCUS_OFFSET,
                                                    widget_anchor_y);
    struct_ConfigGraphicWidget focus_led_cfg{
        .pixel_frame_width = FOCUS_WIDTH,
        .pixel_frame_height = graph_cfg.pixel_frame_height,
        .fg_color = PixelColor::WHITE,
        .bg_color = PixelColor::BLACK};

    this->focus_led = new WidgetFocusIndicator(actual_displayed_model,
                                               display_screen,
                                               focus_led_cfg,
                                               widget_anchor_x,
                                               widget_anchor_y,
                                               false);
    this->focus_led->set_blink_us(200000);
    add_widget(this->square_led);
    add_widget(this->focus_led);

    this->actual_displayed_model->set_change_flag(); // otehrwise nothing is drawn before we act on the rotary encoder
}

MySquareLEDWidgetWithFocus::~MySquareLEDWidgetWithFocus()
{
}

void MySquareLEDWidgetWithFocus::draw()
{
}

void MySquareLEDWidgetWithFocus::draw_refresh()
{
    pr_D4.hi();
    focus_led->draw_refresh();
    pr_D4.lo();
    pr_D5.hi();
    square_led->draw_refresh();
    pr_D5.lo();
}
