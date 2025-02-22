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

void GraphicWidget::draw_refresh()
{
    if (this->actual_displayed_model->has_changed())
    {
        draw();
        this->actual_displayed_model->clear_change_flag();
    }
    if (widgets.size() != 0)
    {
        for (auto &&w : widgets)
            w->draw_refresh();
    }
}

void GraphicWidget::show()
{
    this->graphic_display_screen->show(&this->pixel_frame, this->widget_anchor_x, this->widget_anchor_y);
}

GraphicWidget::GraphicWidget(GraphicDisplayDevice *display_screen,
                             UIModelObject *displayed_object,
                             struct_ConfigGraphicFramebuffer graph_cfg,
                             uint8_t widget_anchor_x,
                             uint8_t widget_anchor_y,
                             bool widget_with_border)
    : GraphicFramebuffer(display_screen, graph_cfg),
      UIWidget(displayed_object, widget_anchor_x, widget_anchor_y)
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

PrintWidget::PrintWidget(PrinterDevice *display_device, UIModelObject *actual_displayed_model)
{
    this->display_device = display_device;
    this->actual_displayed_model = actual_displayed_model;
}

PrintWidget::~PrintWidget()
{
}

void PrintWidget::add_widget(PrintWidget *_sub_widget)
{
    this->widgets.push_back(_sub_widget);
}

void PrintWidget::draw_refresh()
{
    if (widgets.size() != 0)
    {
        for (auto &&w : widgets)
            w->draw_refresh();
    }
    else
    {
        if (this->actual_displayed_model->has_changed())
        {
            draw();
            this->actual_displayed_model->clear_change_flag();
        }
    }
}

TextWidget::TextWidget(GraphicDisplayDevice *device,
                       struct_ConfigTextFramebuffer text_cfg,
                       UIModelObject *displayed_model,
                       uint8_t widget_anchor_x,
                       uint8_t widget_anchor_y,
                       bool widget_with_border)
    : TextFrameBuffer(device, text_cfg),
      UIWidget(displayed_model, widget_anchor_x, widget_anchor_y)
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

void TextWidget::draw_refresh()
{
    if (this->actual_displayed_model->has_changed())
    {
        draw();
        this->actual_displayed_model->clear_change_flag();
    }
    if (widgets.size() != 0)
    {
        for (auto &&w : widgets)
            w->draw_refresh();
    }
}

void TextWidget::draw()
{
    TextFrameBuffer::write();
}

void TextWidget::draw_border(PixelColor color)
{
}

bool UIWidget::blinking_phase_has_changed()
{
    int8_t current_blinking_phase = (time_us_32() / (this->blink_period_us / 2)) % 2;
    bool phase_has_changed = (previous_blinking_phase != current_blinking_phase);
    previous_blinking_phase = current_blinking_phase;
    return phase_has_changed;
}

UIWidget::UIWidget(UIModelObject *actual_displayed_model,
                   uint8_t widget_anchor_x, uint8_t widget_anchor_y)
{
    this->actual_displayed_model = actual_displayed_model;
    this->actual_displayed_model->update_attached_widgets(this);
    this->widget_anchor_x = widget_anchor_x;
    this->widget_anchor_y = widget_anchor_y;
}

UIWidget::~UIWidget()
{
}

void UIWidget::set_blink_us(uint32_t new_blink_period)
{
    this->blink_period_us = new_blink_period;
}

void UIWidget::add_widget(UIWidget *_sub_widget)
{
    this->widgets.push_back(_sub_widget);
}
