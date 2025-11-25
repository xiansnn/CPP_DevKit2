/**
 * @file t_rtos_manager.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "t_rtos_manager.h"
#include "t_rtos_controlled_value.h"
#define UI_ACTIVITY_FLAG (1UL << 0)

my_TestManager::my_TestManager(bool is_wrapable)
    : rtos_UIModelManager(is_wrapable)
{
}

my_TestManager::~my_TestManager()
{
}

void my_TestManager::process_control_event(struct_ControlEventData control_event)
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
