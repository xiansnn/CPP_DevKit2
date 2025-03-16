
#include "widget_horizontal_bar.h"

void WidgetHorizontalBar::convert_level_value_to_px()
{
    px_position = level * level_coef + level_offset;
    px_position = std::min(px_max, std::max(px_min, px_position));
}

void WidgetHorizontalBar::draw()
{
    if (actual_displayed_model->has_changed())
    {
        uint8_t bar_start;
        uint8_t bar_end;

        clear_pixel_buffer();
        get_value_of_interest();
        
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
            rect(bar_start, 0, 1, pixel_frame.pixel_frame_height, true);
        else
            rect(bar_start, 0, bar_end - bar_start, pixel_frame.pixel_frame_height, true);

        draw_border();
        show();
        actual_displayed_model->draw_widget_done();
    }
}

WidgetHorizontalBar::WidgetHorizontalBar(Model *bar_value_model,
                                         GraphicDisplayDevice *display_screen,
                                         int max_value, int min_value,
                                         struct_ConfigGraphicWidget graph_cfg)
    : GraphicWidget(display_screen, graph_cfg, bar_value_model)
{
    this->max_value = max_value;
    this->min_value = min_value;
    this->level = 0;
    this->px_max = graph_cfg.pixel_frame_width;
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
