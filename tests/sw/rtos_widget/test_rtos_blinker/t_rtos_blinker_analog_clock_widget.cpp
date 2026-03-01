#include "t_rtos_blinker_analog_clock_widget.h"

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

struct_ConfigGraphicWidget analog_clock_widget_config = {
    .canvas_width_pixel = CLOCK_SIZE_pixel,
    .canvas_height_pixel = CLOCK_SIZE_pixel,
    .canvas_foreground_color = ColorIndex::WHITE,
    .canvas_background_color = ColorIndex::BLACK,
    .widget_anchor_x = 14,
    .widget_anchor_y = 0,
    .widget_with_border = false,
};

std::map<ClockElementType, std::string> clock_element_to_string{
    {ClockElementType::HOUR, "HOUR"},
    {ClockElementType::MINUTE, "MINUTE"},
    {ClockElementType::SECOND, "SECOND"}};

void AnalogClockWidget::draw_dial()
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

void AnalogClockWidget::draw_clock_hands(int angle_degree, uint length, ColorIndex color)
{
    float angle_rad = (90 - angle_degree) * DEG_TO_RAD;
    uint x_end = CLOCK_X_CENTER + static_cast<int>(length * cosf(angle_rad));
    uint y_end = CLOCK_Y_CENTER - static_cast<int>(length * sinf(angle_rad));
    this->drawer->line(CLOCK_X_CENTER, CLOCK_Y_CENTER, x_end, y_end, color);
}

AnalogClockWidget::~AnalogClockWidget()
{
    delete clock_hour_widget_element;
    delete clock_minute_widget_element;
    delete clock_second_widget_element;
}

AnalogClockWidget::AnalogClockWidget(rtos_Model *actual_displayed_model,
                                     rtos_Blinker *blinker,
                                     struct_ConfigGraphicWidget graph_cfg,
                                     CanvasFormat canvas_format,
                                     rtos_DisplayDevice *display_device)
    : rtos_GraphicWidget(actual_displayed_model,
                         graph_cfg,
                         canvas_format,
                         display_device)
{
    clock_hour_widget_element = new AnalogClockWidgetElement(this,
                                                             blinker,
                                                             ((myMainClock *)actual_displayed_model)->hour,
                                                             ClockElementType::HOUR);
    clock_minute_widget_element = new AnalogClockWidgetElement(this,
                                                               blinker,
                                                               ((myMainClock *)actual_displayed_model)->minute,
                                                               ClockElementType::MINUTE);
    clock_second_widget_element = new AnalogClockWidgetElement(this,
                                                               blinker,
                                                               ((myMainClock *)actual_displayed_model)->second,
                                                               ClockElementType::SECOND);

    this->actual_rtos_displayed_model = actual_displayed_model;
    this->display_device = display_device;
    this->widget_anchor_x = graph_cfg.widget_anchor_x;
    this->widget_anchor_y = graph_cfg.widget_anchor_y;
}
void AnalogClockWidget::draw()
{
    this->drawer->clear_widget();
    get_value_of_interest();
    draw_dial();
    this->clock_hour_widget_element->draw();
    this->clock_minute_widget_element->draw();
    this->clock_second_widget_element->draw();
    this->drawer->draw_border();
}
void AnalogClockWidget::get_value_of_interest()
{ /*
     mandatory because of abstract class implementation, but in this case the clock widget itself does not have a value of interest to retrieve,
     as it is the clock widget elements that retrieve the values of interest (hour, minute, second) from the model.
     So this method can be left empty or used to retrieve any additional information needed for drawing the clock
     that is not already retrieved by the clock widget elements.
   */
}

AnalogClockWidgetElement::AnalogClockWidgetElement(rtos_GraphicWidget *host_widget,
                                                   rtos_Blinker *blinker,
                                                   rtos_Model *actual_displayed_model,
                                                   ClockElementType clock_element_type)
    : rtos_Widget(actual_displayed_model,
                  host_widget->display_device),
      rtos_BlinkingWidget(blinker)
{
    this->host_widget = host_widget;

    this->clock_element_type = clock_element_type;
    switch (this->clock_element_type)
    {
    case ClockElementType::HOUR:
        this->fg_element_color = HOUR_HAND_COLOR_index;
        this->length = HOUR_HAND_LENGTH;
        break;
    case ClockElementType::MINUTE:
        this->fg_element_color = MINUTE_HAND_COLOR_index;
        this->length = MINUTE_HAND_LENGTH;
        break;
    case ClockElementType::SECOND:
        this->fg_element_color = SECOND_HAND_COLOR_index;
        this->length = SECOND_HAND_LENGTH;
        break;
    default:
        break;
    }
    this->bg_element_color = DIAL_BACKGROUND_COLOR_index;
    save_canvas_color();
}

AnalogClockWidgetElement::~AnalogClockWidgetElement()
{
}

void AnalogClockWidgetElement::draw()
{
    get_value_of_interest();
    convert_status_to_blinking_behavior(status);
    ((AnalogClockWidget *)host_widget)->draw_clock_hands(this->angle_degree, this->length, this->fg_element_color);
}

void AnalogClockWidgetElement::get_value_of_interest()
{
    myControlledClockTime *actual_model = (myControlledClockTime *)this->actual_rtos_displayed_model;
    status = actual_model->get_rtos_status();
    myMainClock *main_clock_model = actual_model->parent_model;
    ControlledObjectStatus clock_status = main_clock_model->get_rtos_status();

    switch (this->clock_element_type)
    {
    case ClockElementType::HOUR:

        if (clock_status == ControlledObjectStatus::IS_ACTIVE)
            angle_degree = (main_clock_model->hour->get_value() % 12) * 30 + (main_clock_model->minute->get_value() * 30) / 60;
        else
            angle_degree = (main_clock_model->hour->get_value() % 12) * 30;
        break;
    case ClockElementType::MINUTE:
        if (clock_status == ControlledObjectStatus::IS_ACTIVE)
            angle_degree = main_clock_model->minute->get_value() * 6 + (main_clock_model->second->get_value() * 6) / 60;
        else
            angle_degree = main_clock_model->minute->get_value() * 6;
        break;
    case ClockElementType::SECOND:
        angle_degree = main_clock_model->second->get_value() * 6;
        break;

    default:
        break;
    }
}

void AnalogClockWidgetElement::save_canvas_color()
{
    this->fg_color_backup = this->fg_element_color;
    this->bg_color_backup = DIAL_BACKGROUND_COLOR_index; // assuming black background for simplicity, this should be retrieved from the actual canvas in a real implementation
}

void AnalogClockWidgetElement::restore_canvas_color()
{
    this->fg_element_color = this->fg_color_backup;
}

void AnalogClockWidgetElement::blink()
{
    this->fg_element_color = (blinker->current_blink_phase) ? this->bg_color_backup : this->fg_color_backup;
    if (this->task_handle != nullptr)
        xTaskNotifyGive(this->task_handle);
}

void AnalogClockWidgetElement::set_focus_color()
{
    this->fg_element_color = ColorIndex::CYAN; // example focus color, this can be customized
}