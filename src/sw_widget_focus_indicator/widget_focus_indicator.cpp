/**
 * @file widget_focus_indicator.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-15
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "widget_focus_indicator.h"

WidgetFocusIndicator::WidgetFocusIndicator(UIModelObject *actual_displayed_model,
                                           DisplayDevice *display_screen,
                                           size_t width, size_t height,
                                           uint8_t widget_anchor_x, uint8_t widget_anchor_y, bool with_border)
    : WidgetSquareLed(display_screen,
                      width, height,
                      widget_anchor_x, widget_anchor_y, with_border)
{
    this->actual_displayed_model = actual_displayed_model;
    this->led_is_blinking = false;
    this->led_is_on = true;
}

WidgetFocusIndicator::~WidgetFocusIndicator()
{
}

void WidgetFocusIndicator::draw_refresh()
{
    assert(this->actual_displayed_model != nullptr);
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
            rect(widget_start_x, widget_start_y, widget_width, widget_height, true, PixelColor::WHITE);
        }
        else
        {
            rect(widget_start_x, widget_start_y, widget_width, widget_height, true, PixelColor::BLACK);
        }
        this->display_screen->show(&this->pixel_memory, this->widget_anchor_x, this->widget_anchor_y);
        // this->actual_displayed_model->clear_change_flag(); // if the last widget must clear the model change flag
    }
}
