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
    draw_border();
    show();
}

void WidgetHorizontalBargraph::draw_bar(uint8_t bin_index)
{
    uint8_t bar_start_y = widget_start_y + bar_spacing + bin_index * (bar_height + bar_spacing);
    rect(widget_start_x, bar_start_y, widget_width, bar_height, true, PixelColor::BLACK); // erase the bar area

    uint8_t px = convert_level_value_to_px(((ModelBargraph *)this->actual_displayed_model)->values[bin_index]);
    uint16_t p0 = convert_level_value_to_px(0);

    uint8_t px_start;
    uint8_t px_end;
    if (((ModelBargraph *)this->actual_displayed_model)->values[bin_index] >= 0)
    {
        px_start = p0;
        px_end = px;
    }
    else
    {
        px_start = px;
        px_end = p0;
    }
    if (((ModelBargraph *)this->actual_displayed_model)->values[bin_index] == 0)
        rect(px_start, bar_start_y, 1, bar_height, true);
    else
        rect(px_start, bar_start_y, px_end - px_start, bar_height, true);
}

WidgetHorizontalBargraph::WidgetHorizontalBargraph(ModelBargraph *bargraph_model,
                                                   GraphicDisplayDevice *graphic_display_screen,
                                                   struct_ConfigGraphicFramebuffer graph_cfg,
                                                   uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                                   bool widget_with_border,
                                                   uint8_t _bar_spacing)
    : GraphicWidget(graphic_display_screen, bargraph_model, graph_cfg, widget_anchor_x, widget_anchor_y, widget_with_border)
{
    this->bar_spacing = _bar_spacing;

    uint8_t bin_quantity = ((ModelBargraph *)actual_displayed_model)->number_of_bar;
    int min_value = ((ModelBargraph *)actual_displayed_model)->min_value;
    int max_value = ((ModelBargraph *)actual_displayed_model)->max_value;

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
    : ModelObject()
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
}

uint8_t WidgetVerticalBargraph::convert_level_value_to_py(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(py_max, std::max(py_min, position));
    return position;
}

void WidgetVerticalBargraph::draw()
{
    for (int i = 0; i < ((ModelBargraph *)this->actual_displayed_model)->number_of_bar; i++)
        draw_bar(i);
    draw_border();
    show();
}

void WidgetVerticalBargraph::draw_bar(uint8_t bin_index)
{
    uint8_t bar_start_x = widget_start_x + bar_spacing + bin_index * (bar_width + bar_spacing);
    rect(bar_start_x, widget_start_y, widget_width, bar_height, true, PixelColor::BLACK); // erase the bar area

    uint8_t py = convert_level_value_to_py(((ModelBargraph *)this->actual_displayed_model)->values[bin_index]);
    uint16_t p0 = convert_level_value_to_py(0);

    uint8_t py_start;
    uint8_t py_end;
    if (((ModelBargraph *)this->actual_displayed_model)->values[bin_index] >= 0)
    {
        py_start = py;
        py_end = p0;
    }
    else
    {
        py_start = p0;
        py_end = py;
    }

    if (((ModelBargraph *)this->actual_displayed_model)->values[bin_index] == 0)
        rect(bar_start_x, p0, bar_width, 1, true);
    else
        rect(bar_start_x, py_start, bar_width, py_end - py_start, true);
}

WidgetVerticalBargraph::WidgetVerticalBargraph(ModelBargraph *bargraph_model,
                                               GraphicDisplayDevice *graphic_display_screen,
                                               struct_ConfigGraphicFramebuffer graph_cfg,
                                               uint8_t widget_anchor_x, uint8_t widget_anchor_y,
                                               bool widget_with_border,
                                               uint8_t _bar_spacing)
    : GraphicWidget(graphic_display_screen, bargraph_model, graph_cfg, widget_anchor_x, widget_anchor_y, widget_with_border)
{
    this->bar_spacing = _bar_spacing;

    uint8_t bin_quantity = ((ModelBargraph *)actual_displayed_model)->number_of_bar;
    int min_value = ((ModelBargraph *)actual_displayed_model)->min_value;
    int max_value = ((ModelBargraph *)actual_displayed_model)->max_value;

    this->bar_width = (widget_width - (bin_quantity + 1) * bar_spacing) / bin_quantity; // less than 5 px height is hard to read!

    this->widget_width = this->bar_width * bin_quantity + (bin_quantity + 1) * bar_spacing; // adjust effective widget height to an exact multiple of bin height + bin_spacing

    this->py_max = this->widget_height;
    this->py_min = this->widget_start_y;
    this->bar_height = py_max - py_min;
    this->level_coef = (float)(py_max - py_min) / (min_value - max_value);
    this->level_offset = py_max - level_coef * min_value;
}

WidgetVerticalBargraph::~WidgetVerticalBargraph()
{
}