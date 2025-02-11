/**
 * @file widget.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-10
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "widget.h"

void GraphicWidget::draw_border(PixelColor c)
{
    if (this->widget_with_border)
        rect(0, 0, widget_width + 2 * widget_border_width, widget_height + 2 * widget_border_width);
}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *display_screen,
               UIModelObject *displayed_object,
               struct_ConfigGraphicFramebuffer graph_cfg,
               uint8_t widget_anchor_x,
               uint8_t widget_anchor_y,
               bool widget_with_border)
    : GraphicFramebuffer(display_screen, graph_cfg)
{
    display_screen->check_display_device_compatibility(graph_cfg, widget_anchor_x, widget_anchor_y);

    this->actual_displayed_model = displayed_object;

    this->widget_anchor_x = widget_anchor_x;
    this->widget_anchor_y = widget_anchor_y;
    this->widget_with_border = widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = pixel_memory.frame_width - 2 * widget_border_width;
    widget_height = pixel_memory.frame_height - 2 * widget_border_width;
}

GraphicWidget::~GraphicWidget()
{
}

void GraphicWidget::set_display_screen(GraphicDisplayDevice *_new_display_device)
{
    this->graphic_display_screen = _new_display_device;
}

void GraphicWidget::set_blink_us(uint32_t _blink_period_us)
{
    this->blink_period_us = _blink_period_us;
}

bool GraphicWidget::blinking_phase_has_changed()
{
    int8_t current_blinking_phase = (time_us_32() / (this->blink_period_us / 2)) % 2;
    bool phase_has_changed = (previous_blinking_phase != current_blinking_phase);
    previous_blinking_phase = current_blinking_phase;
    return phase_has_changed;
}

void GraphicWidget::add_widget(GraphicWidget *_sub_widget)
{
    this->widgets.push_back(_sub_widget);
}
