#include "t_rtos_blinking_led_dummy_widgets.h"
#include "t_rtos_blinking_led_main_classes.h"

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_IDLE, "IS_IDLE"},
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

focus_dummy_widget::focus_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

focus_dummy_widget::~focus_dummy_widget()
{
}

void focus_dummy_widget::draw()
{
    get_value_of_interest();
    rtos_UIControlledModel *clock_time = ((myFocusManager *)this->actual_rtos_displayed_model)->managed_rtos_models[current_focus_index];
    std::string name = ((myControlledClockTime *)clock_time)->name;
    printf("focus manager status is [%s], focus is on <%s>\n", status_string.c_str(), name.c_str());
}

void focus_dummy_widget::get_value_of_interest()
{
    myFocusManager *focus_manager = (myFocusManager *)this->actual_rtos_displayed_model;
    this->current_focus_index = focus_manager->get_current_focus_index();
    this->status_string = status_to_string[focus_manager->get_rtos_status()];
}

clock_dummy_widget::clock_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

clock_dummy_widget::~clock_dummy_widget()
{
}

void clock_dummy_widget::draw()
{
    get_value_of_interest();
    printf("clock status is [%s], values are < %2d : %2d : %2d >\n", status_string.c_str(), this->hour_value, this->minute_value, this->second_value);
}

void clock_dummy_widget::get_value_of_interest()
{
    myMainClock *clock = (myMainClock *)this->actual_rtos_displayed_model;
    hour_value = clock->hour.get_value();
    minute_value = clock->minute.get_value();
    second_value = clock->second.get_value();
    status_string = status_to_string[clock->get_rtos_status()];
}

hour_dummy_widget::hour_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

hour_dummy_widget::~hour_dummy_widget()
{
}

void hour_dummy_widget::draw()
{
    get_value_of_interest();
    printf("hour status is [%s], values are < %2d :    :    >\n", status_string.c_str(), this->hour_value);
}

void hour_dummy_widget::get_value_of_interest()
{
    myControlledClockTime *time_hand = (myControlledClockTime *)this->actual_rtos_displayed_model;
    hour_value = time_hand->get_value();
}

minute_dummy_widget::minute_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

minute_dummy_widget::~minute_dummy_widget()
{
}

void minute_dummy_widget::draw()
{
    get_value_of_interest();
    printf("minute status is [%s], values are <    : %2d :    >\n", status_string.c_str(), this->minute_value);
}

void minute_dummy_widget::get_value_of_interest()
{
    myControlledClockTime *time_hand = (myControlledClockTime *)this->actual_rtos_displayed_model;
    minute_value = time_hand->get_value();
}

second_dummy_widget::second_dummy_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

second_dummy_widget::~second_dummy_widget()
{
}

void second_dummy_widget::draw()
{
    get_value_of_interest();
    printf("second status is [%s], values are <    :    : %2d >\n", status_string.c_str(), this->second_value);
}

void second_dummy_widget::get_value_of_interest()
{
    myControlledClockTime *time_hand = (myControlledClockTime *)this->actual_rtos_displayed_model;
    second_value = time_hand->get_value();

}
