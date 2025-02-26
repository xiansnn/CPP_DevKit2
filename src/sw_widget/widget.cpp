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

void GraphicWidget::draw_border(PixelColor color)
{
    if (this->widget_with_border)
        rect(0, 0, widget_width + 2 * widget_border_width, widget_height + 2 * widget_border_width, false, color);
}

void GraphicWidget::show()
{
    this->graphic_display_screen->show(&this->pixel_frame, this->widget_anchor_x, this->widget_anchor_y);
}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *display_screen,
                             Model *displayed_object,
                             struct_ConfigGraphicFramebuffer graph_cfg,
                             uint8_t widget_anchor_x,
                             uint8_t widget_anchor_y,
                             bool widget_with_border)
    : GraphicFramebuffer(display_screen, graph_cfg),
      Widget(displayed_object, widget_anchor_x, widget_anchor_y)
{
    display_screen->check_display_device_compatibility(graph_cfg, widget_anchor_x, widget_anchor_y);

    this->widget_with_border = widget_with_border;
    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = pixel_frame.frame_width - 2 * widget_border_width;
    widget_height = pixel_frame.frame_height - 2 * widget_border_width;
}

GraphicWidget::~GraphicWidget()
{
}

PrintWidget::PrintWidget(PrinterDevice *display_device, Model *actual_displayed_model)
    : Widget(actual_displayed_model, 0, 0)
{
    this->display_device = display_device;
}

PrintWidget::~PrintWidget()
{
}

void PrintWidget::draw_border(PixelColor color)
{
}

TextWidget::TextWidget(GraphicDisplayDevice *device,
                       struct_ConfigTextFramebuffer text_cfg,
                       Model *displayed_model,
                       uint8_t widget_anchor_x,
                       uint8_t widget_anchor_y,
                       bool widget_with_border)
    : TextFramebuffer(device, text_cfg),
      Widget(displayed_model, widget_anchor_x, widget_anchor_y)
{
    this->widget_with_border = widget_with_border;

    this->widget_border_width = (widget_with_border) ? 1 : 0;

    widget_start_x = widget_border_width;
    widget_start_y = widget_border_width;
    widget_width = pixel_frame.frame_width - 2 * widget_border_width;
    widget_height = pixel_frame.frame_height - 2 * widget_border_width;
}

TextWidget::~TextWidget()
{
}

void TextWidget::show()
{
    this->graphic_display_screen->show(&this->pixel_frame, this->widget_anchor_x, this->widget_anchor_y);
}

void TextWidget::draw()
{
    TextFramebuffer::write();
    draw_border();
    show();
}

void TextWidget::draw_border(PixelColor color)
{
    if (this->widget_with_border)
        rect(0, 0, pixel_frame.frame_width, pixel_frame.frame_height, false, color);
}

bool Widget::blinking_phase_has_changed()
{
    int8_t current_blinking_phase = (time_us_32() / (this->blink_period_us / 2)) % 2;
    bool phase_has_changed = (previous_blinking_phase != current_blinking_phase);
    previous_blinking_phase = current_blinking_phase;
    return phase_has_changed;
}

Widget::Widget(Model *actual_displayed_model,
                   uint8_t widget_anchor_x, uint8_t widget_anchor_y)
{
    if (actual_displayed_model != nullptr)
    {
        this->actual_displayed_model = actual_displayed_model;
        this->actual_displayed_model->update_attached_widgets(this);
    }

    this->widget_anchor_x = widget_anchor_x;
    this->widget_anchor_y = widget_anchor_y;
}

Widget::~Widget()
{
}

void Widget::set_blink_us(uint32_t new_blink_period)
{
    this->blink_period_us = new_blink_period;
}

void Widget::add_widget(Widget *_sub_widget)
{
    this->widgets.push_back(_sub_widget);
}
