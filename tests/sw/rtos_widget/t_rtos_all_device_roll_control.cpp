#include "t_rtos_all_device_roll_control.h"

// my_ControlledRollPosition::my_ControlledRollPosition(int _min_value, int _max_value, bool _is_wrappable, int increment)
// :core_IncrementControlledModel(_min_value, _max_value, _is_wrappable, increment), rtos_UIControlledModel()
// {
// }

// my_ControlledRollPosition::~my_ControlledRollPosition()
// {
// }

// void my_ControlledRollPosition::process_control_event(struct_ControlEventData control_event)
// {
// }

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
        this->make_rtos_manager_active();
    }
    else
    {
        if (this->current_active_rtos_model == this)
        {
            switch (control_event.event)
            {
            case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
                this->make_managed_rtos_model_active();
                this->forward_control_event_to_active_managed_model(&control_event);
                break;
            case UIControlEvent::INCREMENT:
                this->increment_focus();
                break;
            case UIControlEvent::DECREMENT:
                this->decrement_focus();
                break;
            case UIControlEvent::TIME_OUT:
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
                this->make_rtos_manager_active();
                break;
            case UIControlEvent::TIME_OUT:
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
