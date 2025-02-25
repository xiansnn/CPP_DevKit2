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

WidgetFocusIndicator::WidgetFocusIndicator(ModelObject *actual_displayed_model,
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
    draw_led();
}

void WidgetFocusIndicator::draw_refresh()
{
    ControlledObjectStatus status = this->actual_displayed_model->get_status();
    switch (status)
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
    WidgetSquareLed::blink_refresh();
    GraphicWidget::draw_refresh();

}


