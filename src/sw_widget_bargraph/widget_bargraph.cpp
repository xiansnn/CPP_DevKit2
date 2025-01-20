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
    uint8_t bar_start_y = widget_start_y + bar_spacing + bin_number * (bar_height + bar_spacing);
    rect(widget_start_x, bar_start_y, widget_width, bar_height, true, FramebufferColor::BLACK); // erase the bar area

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
        rect(bar_start, bar_start_y, 1, bar_height, true);
    else
        rect(bar_start, bar_start_y, bar_end - bar_start, bar_height, true);
}

void WidgetHorizontalBargraph::draw_refresh()
{
    assert(this->actual_displayed_model != nullptr);

    if (this->actual_displayed_model->has_changed())
    {
        this->draw();
        this->draw_border();
        this->display_screen->show(this, this->widget_anchor_x, this->widget_anchor_y);
    }
    this->actual_displayed_model->clear_change_flag();
}

WidgetHorizontalBargraph::WidgetHorizontalBargraph(ModelBargraph *bargraph_model,
                                                   DisplayDevice *display_screen,
                                                   size_t frame_width, size_t frame_height,
                                                   uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                                   bool widget_with_border,
                                                   uint8_t _bar_spacing,
                                                   uint8_t widget_border_width,
                                                   FramebufferFormat framebuffer_format,
                                                   struct_FramebufferText framebuffer_txt_cnf)
    : Widget(display_screen, frame_width, frame_height, widget_anchor_x, widget_anchor_y, widget_with_border, widget_border_width, framebuffer_format, framebuffer_txt_cnf)
{
    set_actual_displayed_object(bargraph_model);
    this->bar_spacing = _bar_spacing;

    uint8_t bin_quantity = ((ModelBargraph *)actual_displayed_model)->number_of_bar;
    int min_value = ((ModelBargraph *)actual_displayed_model)->min_value;
    int max_value = ((ModelBargraph *)actual_displayed_model)->max_value;

    // this->bar_height = std::max(5, (uint8_t)this->widget_height / bin_quantity); // less than 5 px height is hard to read!
    this->bar_height = (widget_height - (bin_quantity + 1) * bar_spacing) / bin_quantity; // less than 5 px height is hard to read!

    this->widget_height = this->bar_height * bin_quantity + (bin_quantity + 1) * bar_spacing; // adjust effective widget height to an exact multiple of bin height + bin_spacing

    this->px_max = this->widget_width;
    this->px_min = this->widget_start_x;
    this->bar_width = px_max - px_min;
    this->level_coef = (float)(px_max - px_min) / (max_value - min_value);
    this->level_offset = px_max - level_coef * max_value;
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

uint8_t WidgetVerticalBargraph::convert_level_value_to_py(int level) // TODO
{
    return 0;
}

void WidgetVerticalBargraph::draw() // TODO
{
}

void WidgetVerticalBargraph::draw_bar(uint8_t bin_number) // TODO
{
}

WidgetVerticalBargraph::WidgetVerticalBargraph(ModelBargraph *bargraph_model,
                                               DisplayDevice *display_screen,
                                               size_t frame_width, size_t frame_height,
                                               uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                               bool widget_with_border,
                                               uint8_t bar_spacing, uint8_t widget_border_width,
                                               FramebufferFormat framebuffer_format, struct_FramebufferText framebuffer_txt_cnf)
    : Widget(display_screen, frame_width, frame_height, widget_anchor_x, widget_anchor_y, widget_with_border, widget_border_width, framebuffer_format, framebuffer_txt_cnf)
{ // TODO
}

WidgetVerticalBargraph::~WidgetVerticalBargraph() // TODO
{
}

void WidgetVerticalBargraph::draw_refresh() // TODO
{
}
