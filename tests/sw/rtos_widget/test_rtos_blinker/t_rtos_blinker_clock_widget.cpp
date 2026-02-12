#include "t_rtos_blinker_clock_widget.h"

#include <cmath>
//--------------- clock related constant--------------------
constexpr uint CLOCK_SIZE_pixel = 100;
constexpr uint CLOCK_RADIUS_pixel = (CLOCK_SIZE_pixel / 2 - 2);
constexpr uint CLOCK_X_CENTER = CLOCK_RADIUS_pixel;
constexpr uint CLOCK_Y_CENTER = CLOCK_RADIUS_pixel;

constexpr uint HOUR_HAND_LENGTH = CLOCK_RADIUS_pixel * 50 / 100;
constexpr ColorIndex HOUR_HAND_COLOR_index = ColorIndex::YELLOW;

constexpr uint MINUTE_HAND_LENGTH = CLOCK_RADIUS_pixel * 70 / 100;
constexpr ColorIndex MINUTE_HAND_COLOR_index = ColorIndex::YELLOW;

constexpr uint SECOND_HAND_LENGTH = CLOCK_RADIUS_pixel * 90 / 100;
constexpr ColorIndex SECOND_HAND_COLOR_index = ColorIndex::RED;

constexpr uint DIAL_NUMBER_OF_DIVISION = 12;
constexpr uint DIAL_NUMBER_OF_SUBDIVISION = 5;
constexpr uint DIAL_NUMBER_OF_MARKS = DIAL_NUMBER_OF_DIVISION * DIAL_NUMBER_OF_SUBDIVISION;
constexpr ColorIndex DIAL_FOREGROUND_COLOR_index = ColorIndex::WHITE;
constexpr ColorIndex DIAL_BACKGROUND_COLOR_index = ColorIndex::BLACK;

constexpr float DEG_TO_RAD = 3.14159f / 180.0f;
//--------------------------------------------------------------


struct_ConfigGraphicWidget clock_widget_config = {
    .canvas_width_pixel = CLOCK_SIZE_pixel,
    .canvas_height_pixel = CLOCK_SIZE_pixel,
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 14,
    .widget_anchor_y = 0,
    .widget_with_border = false,
};

struct_ConfigClockWidgetElement hour_widget_element_config{
    .length = HOUR_HAND_LENGTH,
    .color = HOUR_HAND_COLOR_index,
};
struct_ConfigClockWidgetElement minute_widget_element_config{
    .length = MINUTE_HAND_LENGTH,
    .color = MINUTE_HAND_COLOR_index,
};
struct_ConfigClockWidgetElement second_widget_element_config{
    .length = SECOND_HAND_LENGTH,
    .color = SECOND_HAND_COLOR_index,
};

void ClockWidget::draw_dial()
{
    float mark_start = 0.0f;
    for (size_t i = 0; i < DIAL_NUMBER_OF_MARKS; i++)
    {
        float angle_rad = (90 - i * (360.0f / DIAL_NUMBER_OF_MARKS)) * DEG_TO_RAD;
        mark_start = (i % DIAL_NUMBER_OF_SUBDIVISION == 0) ? MINUTE_HAND_LENGTH : SECOND_HAND_LENGTH;
        uint x_start = CLOCK_X_CENTER + static_cast<int>(mark_start * cosf(angle_rad));
        uint y_start = CLOCK_Y_CENTER - static_cast<int>(mark_start * sinf(angle_rad));
        uint x_end = CLOCK_X_CENTER + static_cast<int>(CLOCK_RADIUS_pixel * cosf(angle_rad));
        uint y_end = CLOCK_Y_CENTER - static_cast<int>(CLOCK_RADIUS_pixel * sinf(angle_rad));
        this->drawer->line(x_start, y_start, x_end, y_end, DIAL_FOREGROUND_COLOR_index);
    }
    this->drawer->circle(CLOCK_RADIUS_pixel, CLOCK_X_CENTER, CLOCK_Y_CENTER, false, DIAL_FOREGROUND_COLOR_index);
}

void ClockWidget::draw_clock_hands(int angle_degree, uint length, ColorIndex color)
{
    float angle_rad = (90 - angle_degree) * DEG_TO_RAD;
    uint x_end = CLOCK_X_CENTER + static_cast<int>(length * cosf(angle_rad));
    uint y_end = CLOCK_Y_CENTER - static_cast<int>(length * sinf(angle_rad));
    this->drawer->line(CLOCK_X_CENTER, CLOCK_Y_CENTER, x_end, y_end, color);
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

    uint clock_size_pixel = this->drawer->canvas->canvas_height_pixel;
    this->radius = (clock_size_pixel / 2) - 2;
    this->x_center = CLOCK_RADIUS_pixel;
    this->y_center = CLOCK_RADIUS_pixel;
    this->hour_length = HOUR_HAND_LENGTH;
    this->minute_length = MINUTE_HAND_LENGTH;
    this->second_length = SECOND_HAND_LENGTH;
    this->hour_color = HOUR_HAND_COLOR_index;
    this->minute_color = MINUTE_HAND_COLOR_index;
    this->second_color = SECOND_HAND_COLOR_index;
}
void ClockWidget::draw()
{
    this->drawer->clear_widget();
    get_value_of_interest();
    draw_dial();
    draw_clock_hands(second_angle_degree, SECOND_HAND_LENGTH, SECOND_HAND_COLOR_index);
    draw_clock_hands(minute_angle_degree, MINUTE_HAND_LENGTH, MINUTE_HAND_COLOR_index);
    draw_clock_hands(hour_angle_degree, HOUR_HAND_LENGTH, HOUR_HAND_COLOR_index);
    this->drawer->draw_border();
}
void ClockWidget::get_value_of_interest()
{
    if (actual_rtos_displayed_model != nullptr)
    {
        myMainClock *actual_model = (myMainClock *)this->actual_rtos_displayed_model;
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

ClockWidgetElement::ClockWidgetElement(rtos_GraphicWidget *host_widget, struct_ConfigClockWidgetElement element_cfg)
{
    this->host_widget = host_widget;
    this->color = element_cfg.color;
    this->length = element_cfg.length;
}

ClockWidgetElement::~ClockWidgetElement()
{
}

MinuteWidgetElement::MinuteWidgetElement(rtos_GraphicWidget *host_widget, rtos_Model *actual_displayed_model, struct_ConfigClockWidgetElement element_cfg)
    : rtos_Widget(actual_displayed_model, host_widget->display_device), ClockWidgetElement(host_widget, element_cfg)
{
}

MinuteWidgetElement::~MinuteWidgetElement()
{
}

void MinuteWidgetElement::draw()
{
    get_value_of_interest();
    convert_status_to_blinking_behavior(status);
    ((ClockWidget *)host_widget)->draw_clock_hands(this->angle_degree, this->length, this->color);
}

void MinuteWidgetElement::get_value_of_interest()
{
    myControlledClockTime *actual_model = (myControlledClockTime *)this->actual_rtos_displayed_model;
    name = actual_model->name;
    status = actual_model->get_rtos_status();

    myMainClock *main_clock_model = actual_model->parent_model;
    ControlledObjectStatus clock_status = main_clock_model->get_rtos_status();
    if (clock_status == ControlledObjectStatus::IS_ACTIVE)
    {
        angle_degree = main_clock_model->minute.get_value() * 6 + (main_clock_model->second.get_value() * 6) / 60;
    }
    else
    {
        angle_degree = main_clock_model->minute.get_value() * 6;
    }
}

void MinuteWidgetElement::save_canvas_color()
{
    this->fg_color_backup = this->color;
    this->bg_color_backup = ColorIndex::BLACK; // assuming black background for simplicity, this should be retrieved from the actual canvas in a real implementation
}

void MinuteWidgetElement::restore_canvas_color()
{
    this->color = this->fg_color_backup;
}

void MinuteWidgetElement::blink()
{
    this->color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;

    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void MinuteWidgetElement::set_focus_color()
{
    this->color = ColorIndex::CYAN; // example focus color, this can be customized
}

SecondWidgetElement::SecondWidgetElement(rtos_GraphicWidget *host_widget, rtos_Model *actual_displayed_model, struct_ConfigClockWidgetElement element_cfg)
    : rtos_Widget(actual_displayed_model, host_widget->display_device), ClockWidgetElement(host_widget, element_cfg)
{
}

SecondWidgetElement::~SecondWidgetElement()
{
}

void SecondWidgetElement::draw()
{
    get_value_of_interest();
    convert_status_to_blinking_behavior(status);
    ((ClockWidget *)host_widget)->draw_clock_hands(angle_degree, this->length, this->color);
}

void SecondWidgetElement::get_value_of_interest()
{
    myControlledClockTime *actual_model = (myControlledClockTime *)this->actual_rtos_displayed_model;
    name = actual_model->name;
    status = actual_model->get_rtos_status();

    myMainClock *main_clock_model = actual_model->parent_model;
    ControlledObjectStatus clock_status = main_clock_model->get_rtos_status();
    angle_degree = actual_model->get_value() * 6;
}

void SecondWidgetElement::save_canvas_color()
{
    this->fg_color_backup = this->color;
    this->bg_color_backup = ColorIndex::BLACK; // assuming black background for simplicity, this should be retrieved from the actual canvas in a real implementation
}

void SecondWidgetElement::restore_canvas_color()
{
    this->color = this->fg_color_backup;
}

void SecondWidgetElement::blink()
{
    this->color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;

    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void SecondWidgetElement::set_focus_color()
{
    this->color = ColorIndex::CYAN; // example focus color, this can be customized
}

HourWidgetElement::HourWidgetElement(rtos_GraphicWidget *host_widget, rtos_Model *actual_displayed_model, struct_ConfigClockWidgetElement element_cfg)
    : rtos_Widget(actual_displayed_model, host_widget->display_device), ClockWidgetElement(host_widget, element_cfg)
{
}

HourWidgetElement::~HourWidgetElement()
{
}

void HourWidgetElement::draw()
{
    get_value_of_interest();
    convert_status_to_blinking_behavior(status);
    ((ClockWidget *)host_widget)->draw_clock_hands(angle_degree, this->length, this->color);
}

void HourWidgetElement::get_value_of_interest()
{
    myControlledClockTime *actual_model = (myControlledClockTime *)this->actual_rtos_displayed_model;
    name = actual_model->name;
    status = actual_model->get_rtos_status();

    myMainClock *main_clock_model = actual_model->parent_model;
    ControlledObjectStatus clock_status = main_clock_model->get_rtos_status();
    if (clock_status == ControlledObjectStatus::IS_ACTIVE)
    {
        angle_degree = (main_clock_model->hour.get_value() % 12) * 30 + (main_clock_model->minute.get_value() * 30) / 60;
    }
    else
    {
        angle_degree = (main_clock_model->hour.get_value() % 12) * 30;
    }
}

void HourWidgetElement::save_canvas_color()
{
    this->fg_color_backup = this->color;
    this->bg_color_backup = ColorIndex::BLACK; // assuming black background for simplicity, this should be retrieved from the actual canvas in a real implementation
}

void HourWidgetElement::restore_canvas_color()
{
    this->color = this->fg_color_backup;
}

void HourWidgetElement::blink()
{
    this->color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;

    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void HourWidgetElement::set_focus_color()
{
    this->color = ColorIndex::CYAN; // example focus color, this can be customized
}
