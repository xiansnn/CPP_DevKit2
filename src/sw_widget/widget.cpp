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

void Widget::draw_border(PixelColor c) 
{
    if (this->widget_with_border)
        rect(0, 0, widget_width + 2 * widget_border_width, widget_height + 2 * widget_border_width);
}

Widget::Widget(GraphicDisplayDevice *_display_screen,
               struct_ConfigGraphicFramebuffer graph_cfg,
               uint8_t _widget_anchor_x,
               uint8_t _widget_anchor_y,
               bool _widget_with_border )
    : GraphicFramebuffer(_display_screen, graph_cfg)
{
    assert(graph_cfg.frame_height % 8 == 0);    // TODO a porter dans ssd1306 check widget height limitation
    assert(_widget_anchor_y % 8 == 0); // TODO a porter dans ssd1306 check widget anchor y limitation
    this->widget_anchor_x = _widget_anchor_x;
    this->widget_anchor_y = _widget_anchor_y;
    this->widget_with_border = _widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = pixel_memory.frame_width - 2 * widget_border_width;
    widget_height = pixel_memory.frame_height - 2 * widget_border_width;
}

Widget::~Widget()
{
}

void Widget::set_actual_displayed_object(UIModelObject *displayed_object)
{
    this->actual_displayed_model = displayed_object;
}

void Widget::set_display_screen(GraphicDisplayDevice *_new_display_device)
{
    this->graphic_display_screen = _new_display_device;
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
