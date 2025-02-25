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
    get_value_of_interest();
    uint8_t bar_start;
    uint8_t bar_end;
    rect(0, 0, pixel_frame.frame_width, pixel_frame.frame_height, true, PixelColor::BLACK); // clear the full framebuffer
    
    draw_border();
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
        rect(bar_start, 0, 1, pixel_frame.frame_height, true);
    else
        rect(bar_start, 0, bar_end - bar_start, pixel_frame.frame_height, true);

    show();
}

WidgetHorizontalBar::WidgetHorizontalBar(Model *bar_value_model,
                                         GraphicDisplayDevice *display_screen,
                                         int max_value, int min_value,
                                         struct_ConfigGraphicFramebuffer graph_cfg,
                                         uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                         bool widget_with_border)
    : GraphicWidget(display_screen, bar_value_model, graph_cfg, widget_anchor_x, widget_anchor_y, widget_with_border)
{
    this->max_value = max_value;
    this->min_value = min_value;
    this->level = 0;
    this->px_max = graph_cfg.frame_width;
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
