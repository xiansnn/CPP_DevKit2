#include "t_rtos_all_device_roll_control.h"
#include "t_rtos_all_device_defines.h"

#include <map>
std::map<UIControlEvent, std::string> event_to_string{
    {UIControlEvent::NONE, "NONE"},
    {UIControlEvent::PUSH, "PUSH"},
    {UIControlEvent::DOUBLE_PUSH, "DOUBLE_PUSH"},
    {UIControlEvent::LONG_PUSH, "LONG_PUSH"},
    {UIControlEvent::RELEASED_AFTER_LONG_TIME, "RELEASED_AFTER_LONG_TIME"},
    {UIControlEvent::RELEASED_AFTER_SHORT_TIME, "RELEASED_AFTER_SHORT_TIME"},
    {UIControlEvent::INCREMENT, "INCREMENT"},
    {UIControlEvent::DECREMENT, "DECREMENT"},
    {UIControlEvent::TIME_OUT, "TIME_OUT"}};

my_ControlledRollPosition::my_ControlledRollPosition(std::string name)
    : rtos_UIControlledModel()
{
    this->name = name;
}

my_ControlledRollPosition::~my_ControlledRollPosition()
{
}

void my_ControlledRollPosition::process_control_event(struct_ControlEventData control_event)
{
    switch (control_event.gpio_number)
    {
    case DUMMY_GPIO_FOR_PERIODIC_EVOLUTION:
        printf("[%s]: %s from DUMMY_GPIO_FOR_PERIODIC_EVOLUTION\n", this->name.c_str(), event_to_string[control_event.event].c_str());
        break;
    case ENCODER_CLK_GPIO:
        printf("[%s]: %s from ENCODER_CLK_GPIO\n", this->name.c_str(), event_to_string[control_event.event].c_str());
        break;
    case CENTRAL_SWITCH_GPIO:
        printf("[%s]: %s from CENTRAL_SWITCH_GPIO\n", this->name.c_str(), event_to_string[control_event.event].c_str());
        break;
    default:
        break;
    }
}

my_PositionController::my_PositionController(bool is_wrapable)
    : rtos_UIModelManager(is_wrapable)
{
}

my_PositionController::~my_PositionController()
{
}

void my_PositionController::process_control_event(struct_ControlEventData control_event)
{
    if (this->get_rtos_status() == ControlledObjectStatus::IS_IDLE)
    {
        printf("position_controller: WAKE_UP\n");
        this->make_rtos_manager_active();
    }
    else
    {
        if (this->current_active_rtos_model == this)
        {
            switch (control_event.event)
            {
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                printf("position_controller focus on [%s]: RELEASED_AFTER_SHORT_TIME\n", ((my_ControlledRollPosition *)managed_rtos_models[get_current_focus_index()])->name.c_str());
                this->make_managed_rtos_model_active();
                this->forward_control_event_to_active_managed_model(&control_event);
                break;
            case UIControlEvent::INCREMENT:
                this->increment_focus();
                printf("position_controller focus on [%s]: INCREMENT\n", ((my_ControlledRollPosition *)managed_rtos_models[get_current_focus_index()])->name.c_str());
                break;
            case UIControlEvent::DECREMENT:
                this->decrement_focus();
                printf("position_controller focus on [%s]: DECREMENT\n", ((my_ControlledRollPosition *)managed_rtos_models[get_current_focus_index()])->name.c_str());
                break;
            case UIControlEvent::TIME_OUT:
                printf("position_controller focus on [%s]: TIME_OUT\n", ((my_ControlledRollPosition *)managed_rtos_models[get_current_focus_index()])->name.c_str());
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                break;
            default:
                break;
            }
        }
        else
        {
            switch (control_event.event)
            {
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                printf("make_rtos_manager_active(): RELEASED_AFTER_SHORT_TIME\n");
                this->make_rtos_manager_active();
                break;
            case UIControlEvent::TIME_OUT:
                printf("controlled_position: TIME_OUT\n");
                this->current_active_rtos_model->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                this->current_active_rtos_model = this;
                this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
                break;
            default:
                this->forward_control_event_to_active_managed_model(&control_event);
                break;
            }
        }
    }
}
