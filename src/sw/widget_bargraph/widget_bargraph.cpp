
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
    if (actual_displayed_model->has_changed())
    {
        clear_pixel_buffer();
        get_value_of_interest();
        for (int i = 0; i < number_of_bar; i++)
            draw_bar(i);
        draw_border();
        show();
        actual_displayed_model->draw_widget_done();
    }
}

void WidgetHorizontalBargraph::get_value_of_interest()
{
    this->values = ((ModelBargraph *)this->actual_displayed_model)->get_values();
}

void WidgetHorizontalBargraph::draw_bar(uint8_t bin_index)
{
    uint8_t bar_start_y = widget_start_y + bar_spacing + bin_index * (bar_height + bar_spacing);

    uint8_t px = convert_level_value_to_px(values[bin_index]);
    uint16_t p0 = convert_level_value_to_px(0);

    uint8_t px_start;
    uint8_t px_end;
    if (values[bin_index] >= 0)
    {
        px_start = p0;
        px_end = px;
    }
    else
    {
        px_start = px;
        px_end = p0;
    }
    if (values[bin_index] == 0)
        rect(px_start, bar_start_y, 1, bar_height, true);
    else
        rect(px_start, bar_start_y, px_end - px_start, bar_height, true);
}

WidgetHorizontalBargraph::WidgetHorizontalBargraph(ModelBargraph *bargraph_model,
                                                   GraphicDisplayDevice *graphic_display_screen,
                                                   struct_ConfigGraphicWidget graph_cfg,
                                                   uint8_t _bar_spacing)
    : GraphicWidget(graphic_display_screen, graph_cfg, bargraph_model)
{
    this->bar_spacing = _bar_spacing;

    this->number_of_bar = ((ModelBargraph *)actual_displayed_model)->number_of_bar;
    int min_value = ((ModelBargraph *)actual_displayed_model)->min_value;
    int max_value = ((ModelBargraph *)actual_displayed_model)->max_value;

    this->bar_height = (widget_height - (number_of_bar + 1) * bar_spacing) / number_of_bar; // less than 5 px height is hard to read!

    this->widget_height = this->bar_height * number_of_bar + (number_of_bar + 1) * bar_spacing; // adjust effective widget height to an exact multiple of bin height + bin_spacing

    this->px_max = this->widget_width;
    this->px_min = this->widget_start_x;
    this->bar_width = px_max - px_min;
    this->level_coef = (float)(px_max - px_min) / (max_value - min_value);
    this->level_offset = px_max - level_coef * max_value;
}

WidgetHorizontalBargraph::~WidgetHorizontalBargraph()
{
}


std::vector<int> ModelBargraph::get_values()
{
    return this->values;
}

void ModelBargraph::update_values(std::vector<int> values)
{
    this->values = values;
    set_change_flag();
}

ModelBargraph::ModelBargraph(size_t number_of_bar, int min_value, int max_value)
    : Model()
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

uint8_t WidgetVerticalBargraph::convert_level_value_to_py(int level)
{
    uint8_t position = level * level_coef + level_offset;
    position = std::min(py_max, std::max(py_min, position));
    return position;
}

void WidgetVerticalBargraph::draw()
{
    if (actual_displayed_model->has_changed())
    {
        clear_pixel_buffer();
        get_value_of_interest();
        for (int i = 0; i < number_of_bar; i++)
            draw_bar(i);
        draw_border();
        show();
        actual_displayed_model->draw_widget_done();
    }
}

void WidgetVerticalBargraph::get_value_of_interest()
{
    this->values = ((ModelBargraph *)this->actual_displayed_model)->get_values();
}

void WidgetVerticalBargraph::draw_bar(uint8_t bin_index)
{
    uint8_t bar_start_x = widget_start_x + bar_spacing + bin_index * (bar_width + bar_spacing);

    uint8_t py = convert_level_value_to_py(values[bin_index]);
    uint16_t p0 = convert_level_value_to_py(0);

    uint8_t py_start;
    uint8_t py_end;
    if (values[bin_index] >= 0)
    {
        py_start = py;
        py_end = p0;
    }
    else
    {
        py_start = p0;
        py_end = py;
    }

    if (values[bin_index] == 0)
        rect(bar_start_x, p0, bar_width, 1, true);
    else
        rect(bar_start_x, py_start, bar_width, py_end - py_start, true);
}

WidgetVerticalBargraph::WidgetVerticalBargraph(ModelBargraph *bargraph_model,
                                               GraphicDisplayDevice *graphic_display_screen,
                                               struct_ConfigGraphicWidget graph_cfg,
                                               uint8_t _bar_spacing)
    : GraphicWidget(graphic_display_screen, graph_cfg, bargraph_model)
{
    this->bar_spacing = _bar_spacing;

    this->number_of_bar = ((ModelBargraph *)actual_displayed_model)->number_of_bar;
    int min_value = ((ModelBargraph *)actual_displayed_model)->min_value;
    int max_value = ((ModelBargraph *)actual_displayed_model)->max_value;

    this->bar_width = (widget_width - (number_of_bar + 1) * bar_spacing) / number_of_bar; // less than 5 px height is hard to read!

    this->widget_width = this->bar_width * number_of_bar + (number_of_bar + 1) * bar_spacing; // adjust effective widget height to an exact multiple of bin height + bin_spacing

    this->py_max = this->widget_height;
    this->py_min = this->widget_start_y;
    this->bar_height = py_max - py_min;
    this->level_coef = (float)(py_max - py_min) / (min_value - max_value);
    this->level_offset = py_max - level_coef * min_value;
}

WidgetVerticalBargraph::~WidgetVerticalBargraph()
{
}