#include "t_rtos_blinker_clock_widget.h"
#include "t_rtos_blinker_main_classes.h"

#include "t_rtos_blinker_console_widgets.h"

#include <cmath>

#define CLOCK_SIZE_PIXEL 100
#define CLOCK_RADIUS_PIXEL (CLOCK_SIZE_PIXEL / 2 - 2)


struct_ConfigGraphicWidget clock_widget_config = {
    .canvas_width_pixel = CLOCK_SIZE_PIXEL,
    .canvas_height_pixel = CLOCK_SIZE_PIXEL,
    .canvas_foreground_color = ColorIndex::YELLOW,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 14,
    .widget_anchor_y = 0,
    .widget_with_border = false,
};

struct_ConfigClockWidgetElement hour_widget_element_config{
    .length = CLOCK_RADIUS_PIXEL * 50 / 100,
    .color = ColorIndex::YELLOW,
};
struct_ConfigClockWidgetElement minute_widget_element_config{
    .length = CLOCK_RADIUS_PIXEL * 70 / 100,
    .color = ColorIndex::YELLOW,
};
struct_ConfigClockWidgetElement second_widget_element_config{
    .length = CLOCK_RADIUS_PIXEL * 90 / 100,
    .color = ColorIndex::RED,
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

    uint clock_size_pixel = this->drawer->canvas->canvas_height_pixel ;
    this->radius = (clock_size_pixel / 2) - 2;
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
    draw_clock_hands(second_angle_degree, second_length, second_color);
    draw_clock_hands(minute_angle_degree, minute_length, minute_color);
    draw_clock_hands(hour_angle_degree, hour_length, hour_color);
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

// void ClockWidgetElement::draw()
// {
//     get_value_of_interest();
//     convert_status_to_blinking_behavior(status);
//     ((ClockWidget*)host_widget)->draw_clock_hands(  second_angle_degree, this->length, this->color);
//     switch (status)
//     {
//     case ControlledObjectStatus::IS_ACTIVE:
//         printf("ClockWidgetElement draw(%s) called with status %s\n", name.c_str(), status_to_string[status].c_str());
//         break;
//     case ControlledObjectStatus::IS_IDLE:
//         printf("ClockWidgetElement draw(%s) called with status %s\n", name.c_str(), status_to_string[status].c_str());
//         break;

//     default:
//         break;
//     }
// }

// void ClockWidgetElement::get_value_of_interest()
// {
//     name = ((myControlledClockTime *)this->actual_rtos_displayed_model)->name;
//     status = ((myControlledClockTime *)this->actual_rtos_displayed_model)->get_rtos_status();
// }

MinuteWidgetElement::MinuteWidgetElement(rtos_GraphicWidget *host_widget, rtos_Model *actual_displayed_model, struct_ConfigClockWidgetElement element_cfg)
    : rtos_Widget(actual_displayed_model, host_widget->display_device), ClockWidgetElement(host_widget, element_cfg)
{
}

MinuteWidgetElement::~MinuteWidgetElement()
{
}

void MinuteWidgetElement::draw()
{
}

void MinuteWidgetElement::get_value_of_interest()
{
}

void MinuteWidgetElement::save_canvas_color()
{
}

void MinuteWidgetElement::restore_canvas_color()
{
}

void MinuteWidgetElement::blink()
{
}

void MinuteWidgetElement::set_focus_color()
{
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
}

void SecondWidgetElement::get_value_of_interest()
{
}

void SecondWidgetElement::save_canvas_color()
{
}

void SecondWidgetElement::restore_canvas_color()
{
}

void SecondWidgetElement::blink()
{
}

void SecondWidgetElement::set_focus_color()
{
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
}

void HourWidgetElement::get_value_of_interest()
{
}

void HourWidgetElement::save_canvas_color()
{
}

void HourWidgetElement::restore_canvas_color()
{
}

void HourWidgetElement::blink()
{
}

void HourWidgetElement::set_focus_color()
{
}
