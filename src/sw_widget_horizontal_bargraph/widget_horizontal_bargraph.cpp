/**
 * @file widget_horizontal_bargraph.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "widget_bargraph.h"
#include "pico/stdlib.h"

uint8_t WidgetHorizontalBargraph::convert_level_value_to_px(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(px_max, std::max(px_min, position));
    return position;
}

void WidgetHorizontalBargraph::draw()
{
    for (int i = 0; i < ((ModelBargraph *)this->actual_displayed_model)->number_of_bar; i++)
        draw_bar(i);
}

void WidgetHorizontalBargraph::draw_bar(uint8_t bin_number)
{
    uint8_t bar_start_y = bin_number * bargraph_bin_height;
    rect(0, bar_start_y, widget_width, bargraph_bin_height, true, FramebufferColor::BLACK); // erase the bar area

    uint8_t px = convert_level_value_to_px(((ModelBargraph *)this->actual_displayed_model)->values[bin_number]);
    uint16_t p0 = convert_level_value_to_px(0);

    uint8_t bar_start;
    uint8_t bar_end;
    if (((ModelBargraph *)this->actual_displayed_model)->values[bin_number] >= 0)
    {
        bar_start = p0;
        bar_end = px;
    }
    else
    {
        bar_start = px;
        bar_end = p0;
    }

    if (((ModelBargraph *)this->actual_displayed_model)->values[bin_number] == 0)
        rect(bar_start, bar_start_y + bargraph_bin_spacing, 1, bargraph_bin_height - bargraph_bin_spacing, true);
    else
        rect(bar_start, bar_start_y + bargraph_bin_spacing, bar_end - bar_start, bargraph_bin_height - 2 * bargraph_bin_spacing, true);
}

void WidgetHorizontalBargraph::draw_refresh()
{
    assert(this->actual_displayed_model != nullptr);

    if (this->actual_displayed_model->has_changed())
    {
        this->draw();
        this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
    }

    this->actual_displayed_model->clear_change_flag();
}

WidgetHorizontalBargraph::WidgetHorizontalBargraph(ModelBargraph *actual_bargraph_model,
                                                   DisplayDevice *display_screen,
                                                   size_t frame_width, size_t frame_height,
                                                   uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                                   bool widget_with_border,
                                                   uint8_t bargraph_bin_spacing,
                                                   uint8_t widget_border_width,
                                                   FramebufferFormat framebuffer_format,
                                                   struct_FramebufferText framebuffer_txt_cnf)
    : Widget(display_screen, frame_width, frame_height, widget_anchor_x, widget_anchor_y, widget_with_border, widget_border_width, framebuffer_format, framebuffer_txt_cnf)
{
    set_actual_displayed_object(actual_bargraph_model);
    this->bargraph_bin_spacing = bargraph_bin_spacing;

    uint8_t bin_quantity = ((ModelBargraph *)actual_bargraph_model)->number_of_bar;
    this->bargraph_bin_height = std::max(5, (uint8_t)this->widget_height / bin_quantity); // less than 5 px height is hard to read!
    this->widget_height = this->bargraph_bin_height * bin_quantity;                       // adjust effective widget height to an exact multiple of bin height

    this->px_max = this->widget_width;
    this->px_min = this->widget_start_x;
    this->bargraph_bin_width = px_max - px_min;
    this->level_coef = (float)(px_max - px_min) / (((ModelBargraph *)actual_bargraph_model)->max_value - ((ModelBargraph *)actual_bargraph_model)->min_value);
    this->level_offset = px_max - level_coef * ((ModelBargraph *)actual_bargraph_model)->max_value;
}

WidgetHorizontalBargraph::~WidgetHorizontalBargraph()
{
}

ModelBargraph::ModelBargraph(size_t number_of_bar, int min_value, int max_value)
    : UIModelObject()
{
    this->max_value = max_value;
    this->min_value = min_value;
    this->number_of_bar = number_of_bar;
    for (size_t i = 0; i < this->number_of_bar; i++)
        this->values.push_back(0);
}

ModelBargraph::~ModelBargraph()
{
}

void ModelBargraph::process_control_event(UIControlEvent event)
{
    this->set_change_flag();
}
