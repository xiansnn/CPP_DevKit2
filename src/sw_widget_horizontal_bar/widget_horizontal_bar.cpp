/**
 * @file widget_horizontal_bar.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "widget_horizontal_bar.h"

void WidgetHorizontalBar::convert_level_value_to_px()
{
    px_position = bar_value * level_coef + level_offset;
    px_position = std::min(px_max, std::max(px_min, px_position));
}

void WidgetHorizontalBar::draw()
{
    rect(0, 0, frame_width, frame_height, true, FramebufferColor::BLACK); // clear the full framebuffer

    uint8_t bar_start;
    uint8_t bar_end;
    if (bar_value >= 0)
    {
        bar_start = level_offset;
        bar_end = px_position;
    }
    else
    {
        bar_start = px_position;
        bar_end = level_offset;
    }

    if (bar_value == 0)
        rect(bar_start, 0, 1, frame_height, true);
    else
        rect(bar_start, 0, bar_end - bar_start, frame_height, true);
}

WidgetHorizontalBar::WidgetHorizontalBar(UIModelObject *bar_value_model,
                                         DisplayDevice *display_screen,
                                         int max_value, int min_value,
                                         size_t frame_width, size_t frame_height,
                                         uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                         bool widget_with_border,
                                         uint8_t widget_border_width,
                                         FramebufferColor color,
                                         FramebufferFormat framebuffer_format)
    : Widget(display_screen, frame_width, frame_height, widget_anchor_x, widget_anchor_y, widget_with_border, 1, framebuffer_format)
{
    this->bar_value_model = bar_value_model;
    this->max_value = max_value;
    this->min_value = min_value;
    this->bar_value = 0;
    this->px_max = frame_width;
    this->px_min = 0;
    this->level_coef = (float)(px_max - px_min) / max_value;
    this->level_offset = px_max - level_coef * max_value;
}

WidgetHorizontalBar::~WidgetHorizontalBar()
{
    delete bar_value_model;
    delete display_screen;
}

void WidgetHorizontalBar::set_value(int value)
{
    this->bar_value = value;
    convert_level_value_to_px();
}

void WidgetHorizontalBar::draw_refresh()
{
    assert(this->bar_value_model != nullptr);
    if (this->bar_value_model->has_changed())
    {
        this->draw();
        this->draw_border();
    }
    this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
    this->bar_value_model->clear_change_flag();
}
