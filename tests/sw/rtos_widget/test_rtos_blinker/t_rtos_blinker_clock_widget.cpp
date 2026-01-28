#include "t_rtos_blinker_clock_widget.h"

#include "t_rtos_blinker_main_classes.h"

#include <cmath>

struct_ConfigGraphicWidget clock_widget_config = {
    .canvas_width_pixel = 100,
    .canvas_height_pixel = 100,
    .canvas_foreground_color = ColorIndex::YELLOW,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 14,
    .widget_anchor_y = 0,
    .widget_with_border = false,
};

void ClockWidget::draw_dial(uint number_of_divisions, uint number_of_subdivisions)
{
    const float deg_to_rad = 3.14159f / 180.0f;
    const uint number_of_marks = number_of_divisions * number_of_subdivisions;
    float mark_start = 0.0f;

    for (size_t i = 0; i < number_of_marks; i++)
    {
        float angle_rad = (90 - i * (360.0f / number_of_marks)) * deg_to_rad;
        mark_start = (i % number_of_subdivisions == 0) ? minute_length : second_length;
        uint x_start = this->x_center + static_cast<int>(mark_start * cosf(angle_rad));
        uint y_start = this->y_center - static_cast<int>(mark_start * sinf(angle_rad));
        uint x_end = this->x_center + static_cast<int>(this->radius * cosf(angle_rad));
        uint y_end = this->y_center - static_cast<int>(this->radius * sinf(angle_rad));
        this->drawer->line(x_start, y_start, x_end, y_end, face_color);
    }
    this->drawer->circle(this->radius, this->x_center, this->y_center, false, face_color);
}

void ClockWidget::draw_clock_hands(int angle_degree, uint length, ColorIndex color)
{
    float angle_rad = (90 - angle_degree) * 3.14159f / 180.0f;
    uint x_end = this->x_center + static_cast<int>(length * cosf(angle_rad));
    uint y_end = this->y_center - static_cast<int>(length * sinf(angle_rad));
    this->drawer->line(this->x_center, this->y_center, x_end, y_end, color);
}

ClockWidget::~ClockWidget()
{
}

ClockWidget::ClockWidget(rtos_Model *actual_displayed_model, struct_ConfigGraphicWidget graph_cfg, CanvasFormat canvas_format, rtos_DisplayDevice *display_device)
    : rtos_GraphicWidget(actual_displayed_model, graph_cfg, canvas_format, display_device)
{
    this->actual_rtos_displayed_model = actual_displayed_model;
    this->display_device = display_device;
    this->widget_anchor_x = graph_cfg.widget_anchor_x;
    this->widget_anchor_y = graph_cfg.widget_anchor_y;

    uint max_dimension = (this->drawer->canvas->canvas_height_pixel > this->drawer->canvas->canvas_width_pixel ? this->drawer->canvas->canvas_height_pixel : this->drawer->canvas->canvas_width_pixel);
    this->radius = (max_dimension / 2) - 2;
    this->x_center = this->radius;
    this->y_center = this->radius;
    this->hour_length = this->radius * 50 / 100;
    this->minute_length = this->radius * 70 / 100;
    this->second_length = this->radius * 90 / 100;
}
void ClockWidget::draw()
{
    get_value_of_interest();
    this->drawer->clear_widget();
    draw_dial(12, 5);
    draw_clock_hands(second_angle_degree, second_length, second_color);
    draw_clock_hands(minute_angle_degree, minute_length, minute_color);
    draw_clock_hands(hour_angle_degree, hour_length, hour_color);
    this->drawer->draw_border();
}
void ClockWidget::get_value_of_interest()
{
    myMainClock *actual_model = (myMainClock *)this->actual_rtos_displayed_model;
    if (actual_rtos_displayed_model != nullptr)
    {
        if (actual_model->get_rtos_status() == ControlledObjectStatus::IS_ACTIVE)
        {
            hour_angle_degree = (actual_model->hour.get_value() % 12) * 30 + (actual_model->minute.get_value() * 30) / 60;
            minute_angle_degree = actual_model->minute.get_value() * 6 + (actual_model->second.get_value() * 6) / 60;
        }
        else
        {
            hour_angle_degree = (actual_model->hour.get_value() % 12) * 30;
            minute_angle_degree = actual_model->minute.get_value() * 6;
        }
        second_angle_degree = actual_model->second.get_value() * 6;
    }
}
