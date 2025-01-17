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


void Widget::draw_border(FramebufferColor c)
{
    rect(0, 0, frame_width, frame_height);
}

Widget::Widget(DisplayDevice *_display_screen,
                   size_t _frame_width,
                   size_t _frame_height,
                   uint8_t _widget_anchor_x,
                   uint8_t _widget_anchor_y,
                   bool _widget_with_border,
                   uint8_t _widget_border_width,
                   FramebufferFormat _framebuffer_format,
                   struct_FramebufferText _framebuffer_txt_cnf)
    : Framebuffer(_frame_width, _frame_height, _framebuffer_format)
{
    assert(_frame_height % 8 == 0);    // check widget height limitation
    assert(_widget_anchor_y % 8 == 0); // check widget anchor y limitation
    this->display_screen = _display_screen;
    this->widget_anchor_x = _widget_anchor_x;
    this->widget_anchor_y = _widget_anchor_y;
    this->widget_with_border = _widget_with_border;
    this->widget_border_width = (widget_with_border) ? _widget_border_width : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = frame_width - 2 * widget_border_width;
    widget_height = frame_height - 2 * widget_border_width;
}

Widget::~Widget()
{
    delete actual_displayed_model;
    delete display_screen;
}

void Widget::set_actual_displayed_object(UIModelObject *displayed_object)
{
    this->actual_displayed_model = displayed_object;
}

void Widget::set_display_screen(DisplayDevice *_new_display_device)
{
    this->display_screen = _new_display_device;
}

void Widget::set_blink_us(uint32_t _blink_period_us)
{
    this->blink_period_us = _blink_period_us;
}

bool Widget::blinking_phase_has_changed()
{
    int8_t current_blinking_phase = (time_us_32() / (this->blink_period_us / 2)) % 2;
    bool phase_has_changed = (previous_blinking_phase != current_blinking_phase);
    previous_blinking_phase = current_blinking_phase;
    return phase_has_changed;
}

void Widget::add_widget(Widget *_sub_widget)
{
    this->widgets.push_back(_sub_widget);
}

// void Widget::draw_refresh()
// {
// }
