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
    px_position = level * level_coef + level_offset;
    px_position = std::min(px_max, std::max(px_min, px_position));
}

void WidgetHorizontalBar::draw()
{
    rect(0, 0, pixel_memory.frame_width, pixel_memory.frame_height, true, PixelColor::BLACK); // clear the full framebuffer

    uint8_t bar_start;
    uint8_t bar_end;
    if (level >= 0)
    {
        bar_start = level_offset;
        bar_end = px_position;
    }
    else
    {
        bar_start = px_position;
        bar_end = level_offset;
    }

    if (level == 0)
        rect(bar_start, 0, 1, pixel_memory.frame_height, true);
    else
        rect(bar_start, 0, bar_end - bar_start, pixel_memory.frame_height, true);
}

WidgetHorizontalBar::WidgetHorizontalBar(UIModelObject *bar_value_model,
                                         DisplayDevice *display_screen,
                                         int max_value, int min_value,
                                         size_t frame_width, size_t frame_height,
                                         uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                         bool widget_with_border,
                                         PixelColor fg_color,
                                         PixelColor bg_color)
    : Widget(display_screen, frame_width, frame_height, widget_anchor_x, widget_anchor_y, widget_with_border, fg_color, bg_color)
{
    this->actual_displayed_model = bar_value_model;
    this->max_value = max_value;
    this->min_value = min_value;
    this->level = 0;
    this->px_max = frame_width;
    this->px_min = 0;
    this->level_coef = (float)(px_max - px_min) / (max_value - min_value);
    this->level_offset = px_max - level_coef * max_value;
}

WidgetHorizontalBar::~WidgetHorizontalBar()
{
}

void WidgetHorizontalBar::set_level(int value)
{
    this->level = value;
    convert_level_value_to_px();
}

void WidgetHorizontalBar::draw_refresh()
{
    assert(this->actual_displayed_model != nullptr);

    if (this->actual_displayed_model->has_changed())
    {
        this->draw();
        this->draw_border();
        this->display_screen->show(&this->pixel_memory, this->widget_anchor_x, this->widget_anchor_y);
        // this->bar_value_model->clear_change_flag(); // this is not the last widget, then we must not clear the change flag
    }
}
