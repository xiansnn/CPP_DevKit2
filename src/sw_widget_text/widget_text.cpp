/**
 * @file widget_text.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "widget_text.h"

void WidgetText::draw_border(FramebufferColor c)
{
    if (this->widget_with_border)
        rect(0, 0, widget_width + 2 * widget_border_width, widget_height + 2 * widget_border_width);
}

void WidgetText::set_display_screen(DisplayDevice *_new_display_device)
{
    this->display_screen = _new_display_device;
}

void WidgetText::set_blink_us(uint32_t blink_period)
{
    this->blink_period_us = blink_period_us;
}

WidgetText::WidgetText(UIModelObject *text_model,
                       DisplayDevice *display_screen,
                       struct_FramebufferText framebuffer_txt_cnf,
                       uint8_t number_of_column,
                       uint8_t number_of_line,
                       uint8_t widget_anchor_x,
                       uint8_t widget_anchor_y,
                       bool widget_with_border,
                       uint8_t widget_border_width,
                       FramebufferFormat framebuffer_format)
    : Framebuffer(number_of_column, number_of_line, framebuffer_txt_cnf)
{
    assert(frame_height % 8 == 0);    // check widget height limitation
    assert(widget_anchor_y % 8 == 0); // check widget anchor y limitation
    this->display_screen = display_screen;
    this->widget_anchor_x = widget_anchor_x;
    this->widget_anchor_y = widget_anchor_y;
    this->widget_with_border = widget_with_border;
    this->widget_border_width = (widget_with_border) ? widget_border_width : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = frame_width - 2 * widget_border_width;
    widget_height = frame_height - 2 * widget_border_width;
}

WidgetText::~WidgetText()
{
    delete actual_displayed_model;
    delete display_screen;
}

void WidgetText::set_actual_displayed_object(UIModelObject *model_text)
{
    this->actual_displayed_model = model_text;
}

void WidgetText::add_widget(WidgetText *_sub_widget)
{
    this->widgets.push_back(_sub_widget);
}

bool WidgetText::blinking_phase_has_changed()
{
    int8_t current_blinking_phase = (time_us_32() / (this->blink_period_us / 2)) % 2;
    bool phase_has_changed = (previous_blinking_phase != current_blinking_phase);
    previous_blinking_phase = current_blinking_phase;
    return phase_has_changed;
}
