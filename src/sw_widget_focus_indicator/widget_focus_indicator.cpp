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
                                           GraphicDisplayDevice *display_screen,
                                           struct_ConfigGraphicFramebuffer graph_cfg,
                                           uint8_t widget_anchor_x, uint8_t widget_anchor_y, bool with_border)
    : WidgetSquareLed(actual_displayed_model, display_screen,
                      graph_cfg,
                      widget_anchor_x, widget_anchor_y, with_border)
{
    this->led_is_blinking = false;
    this->led_is_on = true;
}

WidgetFocusIndicator::~WidgetFocusIndicator()
{
}

void WidgetFocusIndicator::draw()
{
    if (this->led_is_on)
    {
        rect(widget_start_x, widget_start_y, widget_width, widget_height, true, PixelColor::WHITE);
    }
    else
    {
        rect(widget_start_x, widget_start_y, widget_width, widget_height, true, PixelColor::BLACK);
    }
    this->graphic_display_screen->show(&this->pixel_frame, this->widget_anchor_x, this->widget_anchor_y);
}


