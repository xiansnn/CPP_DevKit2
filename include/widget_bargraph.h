/**
 * @file widget_horizontal_bargraph.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "framebuffer.h"
#include "widget.h"
#include "display_device.h"

#include "ui_core.h"

class ModelBargraph : public UIModelObject
{
private:
public:
    std::vector<int> values;
    size_t number_of_bar;
    int min_value;
    int max_value;
    ModelBargraph(size_t number_of_bar, int min_value, int max_value);
    ~ModelBargraph();
    void process_control_event(UIControlEvent event = UIControlEvent::NONE);
};

class WidgetHorizontalBargraph : public Widget
{
private:
    uint8_t bargraph_bin_spacing;
    uint8_t bargraph_bin_height;
    uint8_t bargraph_bin_width;

    uint8_t px_max;
    uint8_t px_min;
    float level_coef;
    int level_offset;

    uint8_t convert_level_value_to_px(int level);
    void draw();
    void draw_bar(uint8_t bin_number);

protected:

public:
    WidgetHorizontalBargraph(ModelBargraph *actual_bargraph_model,
                             DisplayDevice *display_screen,
                             size_t frame_width,
                             size_t frame_height,
                             uint8_t widget_anchor_x,
                             uint8_t widget_anchor_y,
                             bool widget_with_border,
                             uint8_t bargraph_bin_spacing = 1,
                             uint8_t widget_border_width = 1,
                             FramebufferFormat framebuffer_format = FramebufferFormat::MONO_VLSB,
                             struct_FramebufferText framebuffer_txt_cnf = {.font = font_8x8});
    ~WidgetHorizontalBargraph();
    void draw_refresh();
};
