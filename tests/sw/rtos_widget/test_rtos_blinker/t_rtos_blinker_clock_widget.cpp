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
    this->drawer->clear_widget();
    get_value_of_interest();
    draw_dial(12, 5);
    convert_status_to_blinking_behavior(this->clock_status);
    draw_clock_hands(second_angle_degree, second_length, second_color);
    draw_clock_hands(minute_angle_degree, minute_length, minute_color);
    draw_clock_hands(hour_angle_degree, hour_length, hour_color);
    this->drawer->draw_border();
}
void ClockWidget::get_value_of_interest()
{
    current_controlled_hand = CurrentControlledHand::none;
    if (actual_rtos_displayed_model != nullptr)
    {
        myMainClock *actual_model = (myMainClock *)this->actual_rtos_displayed_model;
        if (actual_model->hour.get_rtos_status() == ControlledObjectStatus::IS_ACTIVE)
            current_controlled_hand = CurrentControlledHand::hour;
        else if (actual_model->minute.get_rtos_status() == ControlledObjectStatus::IS_ACTIVE)
            current_controlled_hand = CurrentControlledHand::minute;
        else if (actual_model->second.get_rtos_status() == ControlledObjectStatus::IS_ACTIVE)
            current_controlled_hand = CurrentControlledHand::second;

        this->clock_status = actual_model->get_rtos_status();
        if (this->clock_status == ControlledObjectStatus::IS_ACTIVE)
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

void ClockWidget::save_canvas_color()
{
    this->fg_color_backup = this->drawer->canvas->fg_color;
    this->bg_color_backup = this->drawer->canvas->bg_color;
    this->hour_color_bckup = this->hour_color;
    this->minute_color_bckup = this->minute_color;
    this->second_color_bckup = this->second_color;
}

void ClockWidget::restore_canvas_color()
{
    this->drawer->canvas->fg_color = this->fg_color_backup;
    this->drawer->canvas->bg_color = this->bg_color_backup;
    this->hour_color = this->hour_color_bckup;
    this->minute_color = this->minute_color_bckup;
    this->second_color = this->second_color_bckup;
}

void ClockWidget::blink()
{
    this->drawer->canvas->fg_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;
    // this->drawer->canvas->bg_color = (blinker->current_blink_phase) ? this->fg_color_backup : this->bg_color_backup;
    switch (current_controlled_hand)
    {
    case CurrentControlledHand::hour:
        this->hour_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->hour_color_bckup;
        break;
    case CurrentControlledHand::minute:
        this->minute_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->minute_color_bckup;
        break;
    case CurrentControlledHand::second:
        this->second_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->second_color_bckup;
        break;

    default:
        break;
    }
    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void ClockWidget::set_focus_color()
{
    this->drawer->canvas->fg_color = this->bg_color_backup;
    this->drawer->canvas->bg_color = this->fg_color_backup;
    this->hour_color = this->bg_color_backup;
    this->minute_color = this->bg_color_backup;
    this->second_color = this->bg_color_backup;
}
