#include "t_rtos_blinker_console_widgets.h"
#include "t_rtos_blinker_main_classes.h"

std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_IDLE, "IS_IDLE"},
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

focus_console_widget::focus_console_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

focus_console_widget::~focus_console_widget()
{
}

void focus_console_widget::draw()
{
    get_value_of_interest();
    rtos_UIControlledModel *clock_time = ((myClockController *)this->actual_rtos_displayed_model)->managed_rtos_models[current_focus_index];
    std::string name = ((myControlledClockTime *)clock_time)->name;
    printf("manager status is [%s], focus is on <%s>\n", status_string.c_str(), name.c_str());
}

void focus_console_widget::get_value_of_interest()
{
    myClockController *focus_manager = (myClockController *)this->actual_rtos_displayed_model;
    this->current_focus_index = focus_manager->get_current_focus_index();
    this->status_string = status_to_string[focus_manager->get_rtos_status()];
}

clock_console_widget::clock_console_widget(rtos_Model *actual_displayed_model, rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

clock_console_widget::~clock_console_widget()
{
}

void clock_console_widget::draw()
{
    get_value_of_interest();
    printf("\t\t\t\t\t\t\t\t\t\tclock is [%s], time is < %2d : %2d : %2d >\n", status_string.c_str(), this->hour_value, this->minute_value, this->second_value);
}

void clock_console_widget::get_value_of_interest()
{
    myMainClock *clock = (myMainClock *)this->actual_rtos_displayed_model;
    hour_value = clock->hour.get_value();
    minute_value = clock->minute.get_value();
    second_value = clock->second.get_value();
    status_string = status_to_string[clock->get_rtos_status()];
}
