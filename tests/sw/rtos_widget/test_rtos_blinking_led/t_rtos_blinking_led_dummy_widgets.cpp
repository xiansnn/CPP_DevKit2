#include "t_rtos_blinking_led_dummy_widgets.h"
#include "t_rtos_blinking_led_main_classes.h"









std::map<ControlledObjectStatus, std::string> status_to_string{
    {ControlledObjectStatus::IS_IDLE, "IS_IDLE"},
    {ControlledObjectStatus::IS_WAITING, "IS_WAITING"},
    {ControlledObjectStatus::HAS_FOCUS, "HAS_FOCUS"},
    {ControlledObjectStatus::IS_ACTIVE, "IS_ACTIVE"}};

focus_dummy_widget::focus_dummy_widget(rtos_Model *actual_displayed_model,  rtos_DisplayDevice *display_device)
    : rtos_Widget(actual_displayed_model, display_device)
{
}

focus_dummy_widget::~focus_dummy_widget()
{
}

void focus_dummy_widget::draw()
{
    get_value_of_interest();
    rtos_UIControlledModel *cm = ((myFocusManager *)this->actual_rtos_displayed_model)->managed_rtos_models[current_focus_index];
    std::string name = ((myControlledClockTime *)cm)->name;
    printf("focus manager[%s], pointing to %s\n", status.c_str(), name.c_str());
}

void focus_dummy_widget::get_value_of_interest()
{
    myFocusManager *displayed_model = (myFocusManager *)this->actual_rtos_displayed_model;
    this->current_focus_index = displayed_model->get_current_focus_index();
    this->status = status_to_string[displayed_model->get_rtos_status()];
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
    printf("[%s] %2d : %2d : %2d\n", status.c_str(), this->hour, this->minute, this->second);
}

void clock_dummy_widget::get_value_of_interest()
{

    myMainClock *displayed_model = (myMainClock *)this->actual_rtos_displayed_model;
    hour = displayed_model->hour.get_value();
    minute = displayed_model->minute.get_value();
    second = displayed_model->second.get_value();
    this->status = status_to_string[displayed_model->get_rtos_status()];
}
