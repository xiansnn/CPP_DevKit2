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

my_TestManager::my_TestManager(bool is_wrapable)
    : rtos_UIModelManager(is_wrapable)
{
}

my_TestManager::~my_TestManager()
{
}

void my_TestManager::process_control_event(struct_ControlEventData control_event)
{
    UIControlEvent _event = control_event.event;

    if ((this->get_rtos_status() == ControlledObjectStatus::IS_IDLE) and
        ((_event == UIControlEvent::RELEASED_AFTER_SHORT_TIME) or
         (_event == UIControlEvent::INCREMENT) or
         (_event == UIControlEvent::DECREMENT))) // wake up manager
    {
        this->make_rtos_manager_active();
        printf("manager wakeup \n");
    }
    else if (this->current_active_rtos_model == this)
    {
        switch (_event)
        {
        case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
            this->make_managed_rtos_model_active();
            printf("RELEASED_AFTER_SHORT_TIME->manager ; manager status =%d, focus index = %d \n", get_rtos_status(), this->get_current_focus_index());
            this->forward_control_event_to_active_managed_model(&control_event);
            break;
        case UIControlEvent::INCREMENT:
            this->increment_focus();
            printf("INCREMENT->manager ; manager status =%d, focus index = %d \n", get_rtos_status(), this->get_current_focus_index());
            break;
        case UIControlEvent::DECREMENT:
            this->decrement_focus();
            printf("DECREMENT->manager ; manager status =%d, focus index = %d \n", get_rtos_status(), this->get_current_focus_index());
            break;
        case UIControlEvent::TIME_OUT:
            this->update_rtos_status(ControlledObjectStatus::IS_IDLE);
            printf("TIME_OUT->manager ; manager status =%d, focus index = %d \n", get_rtos_status(), this->get_current_focus_index());
            break;
        default:
            break;
        }
    }
    else
    {
        switch (_event)
        {
        case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
            this->make_rtos_manager_active();
            printf("RELEASED_AFTER_SHORT_TIME->manager becomes ACTIVE ; manager status =%d, focus index = %d\n ", get_rtos_status(), this->get_current_focus_index());
            break;

        default:
            this->forward_control_event_to_active_managed_model(&control_event);
            break;
        }
    }
}
