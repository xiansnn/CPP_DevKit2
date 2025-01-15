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

/**
 * @brief MySquareLedWidget : Example of final implementation of WidgetSquareLed
 */
class MySquareLedWidget : public WidgetSquareLed
{
private:
    // /// @brief the variable that stores the reference to the actual model object.
    // MySquareLedModel *actual_displayed_model = nullptr;

public:
    MySquareLedWidget(MySquareLedModel *actual_displayed_model,
                      DisplayDevice *display_screen,
                      size_t width,
                      size_t height,
                      uint8_t widget_anchor_x,
                      uint8_t widget_anchor_y);
    ~MySquareLedWidget();
    void draw_refresh();
};
MySquareLedWidget::MySquareLedWidget(MySquareLedModel *actual_displayed_model,
                                     DisplayDevice *display_screen,
                                     size_t width,
                                     size_t height,
                                     uint8_t widget_anchor_x,
                                     uint8_t widget_anchor_y)
    : WidgetSquareLed(display_screen, width, height, widget_anchor_x, widget_anchor_y)
{
    this->actual_displayed_model = actual_displayed_model;
    this->led_is_blinking = false;
    this->led_is_on = true;
}

MySquareLedWidget::~MySquareLedWidget()
{
}
/**
 * @brief This function implements a special draw_refresh that takes into account the on/off status of the model.
 *
 * It insures that the widget consumes processing time only when its on/off status has changed.
 * The logic of the visualisation :
 *  - if the displayed model my_bool_value is true(resp. false), the widget led is on(resp. off)
 */
void MySquareLedWidget::draw_refresh()
{
    assert(this->actual_displayed_model != nullptr);
    {
        /// main step of the function
        if (this->actual_displayed_model->has_changed())
        {
            if (((MySquareLedModel *)this->actual_displayed_model)->my_bool_value)
            {
                rect(widget_start_x, widget_start_y, widget_width, widget_height, true, FramebufferColor::WHITE);
            }
            else
            {
                rect(widget_start_x, widget_start_y, widget_width, widget_height, true, FramebufferColor::BLACK);
                draw_border();
            }
            this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
        }
    }
}

class MySquareLEDWidgetWithFocus : Widget
{
private:
    MySquareLedWidget *square_led;
    WidgetFocusIndicator *focus_led;

public:
    MySquareLEDWidgetWithFocus(MySquareLedModel *actual_displayed_model,
                               DisplayDevice *display_screen,
                               size_t width,
                               size_t height,
                               uint8_t widget_anchor_x,
                               uint8_t widget_anchor_y);
    ~MySquareLEDWidgetWithFocus();
    void draw_refresh();
};

MySquareLEDWidgetWithFocus::MySquareLEDWidgetWithFocus(MySquareLedModel *actual_displayed_model,
                                                       DisplayDevice *display_screen,
                                                       size_t width,
                                                       size_t height,
                                                       uint8_t widget_anchor_x,
                                                       uint8_t widget_anchor_y)
                : Widget(display_screen, width, height,widget_anchor_x,widget_anchor_y,true)
{
    this->square_led = new MySquareLedWidget(actual_displayed_model,
                                             display_screen,
                                             width - 8,
                                             height,
                                             widget_anchor_x + 8,
                                             widget_anchor_y);
    this->focus_led = new WidgetFocusIndicator(actual_displayed_model,
                                               display_screen,
                                               5,
                                               height,
                                               widget_anchor_x,
                                               widget_anchor_y,
                                               false);
    this->focus_led->set_blink_us(200000);
    add_widget(this->square_led);
    add_widget(this->focus_led);
}

MySquareLEDWidgetWithFocus::~MySquareLEDWidgetWithFocus()
{
}

void MySquareLEDWidgetWithFocus::draw_refresh()
{
    // this->h_bar_widget->set_value(this->bar_value_model->get_value());
    for (auto &&w : this->widgets)
    {
        w->draw_refresh();
    }
    this ->actual_displayed_model->clear_change_flag();
}

/**
 * @brief MyFocusLedWidget : this is a special led widget used to show the status of the model.
 */
class MyFocusLedWidget : public WidgetSquareLed
{
private:
    /// @brief the variable that stores the reference to the actual model object.
    MySquareLedModel *actual_displayed_model = nullptr;

public:
    MyFocusLedWidget(MySquareLedModel *actual_displayed_model,
                     DisplayDevice *display_screen,
                     size_t width,
                     size_t height,
                     uint8_t widget_anchor_x,
                     uint8_t widget_anchor_y);
    ~MyFocusLedWidget();
    void draw_refresh();
};
MyFocusLedWidget::MyFocusLedWidget(MySquareLedModel *actual_displayed_model,
                                   DisplayDevice *display_screen,
                                   size_t width, size_t height,
                                   uint8_t widget_anchor_x, uint8_t widget_anchor_y)
    : WidgetSquareLed(display_screen, width, height, widget_anchor_x, widget_anchor_y, false)
{
    this->actual_displayed_model = actual_displayed_model;
    this->led_is_blinking = false;
    this->led_is_on = true;
}

MyFocusLedWidget::~MyFocusLedWidget()
{
}

/**
 * @brief
 * It insures that the widget consumes processing time only when the model status has changed.
 * The logic of the visualisation :
 *  - if the displayed model has focus, the focus led is on.
 *  - if the model is active, the led is blinking
 *  - if the model is waiting, the led is off.
 *
 * The time-out takes effect after the configured time and force the status to IS_WAITING
 */
void MyFocusLedWidget::draw_refresh()
{
    assert(this->actual_displayed_model != nullptr);
    {
        /// main step of the function
        /// - first process the status of the displayed object
        switch (this->actual_displayed_model->get_status())
        {
        case ControlledObjectStatus::HAS_FOCUS:
            this->blink_off();
            this->light_on();
            break;
        case ControlledObjectStatus::IS_ACTIVE:
            this->blink_on();
            break;
        case ControlledObjectStatus::IS_WAITING:
            this->blink_off();
            this->light_off();
            break;

        default:
            break;
        }
        /// - then widget_blink_refresh() if it is appropriate
        blink_refresh();
        /// - and finally visualise how we've decide to represent the status of the model
        if (this->actual_displayed_model->has_changed())
        {
            if (this->led_is_on)
            {
                rect(widget_start_x, widget_start_y, widget_width, widget_height, true, FramebufferColor::WHITE);
            }
            else
            {
                rect(widget_start_x, widget_start_y, widget_width, widget_height, true, FramebufferColor::BLACK);
            }
            this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
            this->actual_displayed_model->clear_change_flag(); // the last widget must clear the model change flag
        }
    }
}
