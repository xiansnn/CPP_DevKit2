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

WidgetText::WidgetText(UIModelObject *_text_model,
                       DisplayDevice *_display_screen,
                       struct_FramebufferText _framebuffer_txt_cnf,
                       uint8_t _number_of_column,
                       uint8_t _number_of_line,
                       uint8_t _widget_anchor_x,
                       uint8_t _widget_anchor_y,
                       bool _widget_with_border,
                       uint8_t _widget_border_width,
                       FramebufferFormat _framebuffer_format)
    : Framebuffer(_number_of_column, _number_of_line, _framebuffer_txt_cnf)
{
    assert(this->frame_height % 8 == 0); // check widget height limitation
    assert(_widget_anchor_y % 8 == 0);   // check widget anchor y limitation
    this->display_screen = _display_screen;
    this->actual_displayed_model = _text_model;

    this->widget_anchor_x = _widget_anchor_x;
    this->widget_anchor_y = _widget_anchor_y;
    this->widget_with_border = _widget_with_border;
    this->widget_border_width = (_widget_with_border) ? _widget_border_width : 0;

    widget_start_x = this->widget_border_width;
    widget_start_y = this->widget_border_width;
    widget_width = frame_width - 2 * this->widget_border_width;
    widget_height = frame_height - 2 * this->widget_border_width;
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

void WidgetText::draw_refresh()
{
    assert(this->actual_displayed_model != nullptr);

    if (this->actual_displayed_model->has_changed())
    {
        this->print_text(this->text_buffer);
        this->draw_border();
        this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
    }
    this->actual_displayed_model->clear_change_flag();
}

bool WidgetText::blinking_phase_has_changed()
{
    int8_t current_blinking_phase = (time_us_32() / (this->blink_period_us / 2)) % 2;
    bool phase_has_changed = (previous_blinking_phase != current_blinking_phase);
    previous_blinking_phase = current_blinking_phase;
    return phase_has_changed;
}
