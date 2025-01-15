/**
 * @file widget_horizontal_bar.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "ui_core.h"
#include "widget.h"

class WidgetHorizontalBar : public Widget
{
private:
    int level;
    int max_value;
    int min_value;
    uint8_t px_max;
    uint8_t px_min;
    uint8_t px_position;
    float level_coef;
    int level_offset;


    void convert_level_value_to_px();
    void draw();

protected:

public:
    WidgetHorizontalBar(UIModelObject *bar_value_model,
                        DisplayDevice *display_screen,
                        int max_value, int min_value,
                        size_t frame_width, size_t frame_height,
                        uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                        bool widget_with_border = true,
                        uint8_t widget_border_width = 1,
                        FramebufferColor color = FramebufferColor::WHITE,
                        FramebufferFormat framebuffer_format = FramebufferFormat::MONO_VLSB);

    ~WidgetHorizontalBar();
    void set_value(int value);
    virtual void draw_refresh();
};
