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

my_TestManager::my_TestManager(bool is_wrappable)
    : UIModelManager(is_wrappable), rtos_UIModelManager()
{
}

my_TestManager::~my_TestManager()
{
}

void my_TestManager::process_control_event(UIControlEvent _event)
{
    if (this->current_active_model == this)
    {
        switch (_event)
        {
        case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
            this->make_managed_model_active();
            this->draw_refresh_all_attached_widgets();
            this->notify_current_active_managed_model(_event);
            break;
        case UIControlEvent::INCREMENT:
            this->increment_focus();
            this->draw_refresh_all_attached_widgets();
            break;
        case UIControlEvent::DECREMENT:
            this->decrement_focus();
            this->draw_refresh_all_attached_widgets();
            break;
        case UIControlEvent::TIME_OUT:
            this->update_status(ControlledObjectStatus::IS_WAITING);
            this->draw_refresh_all_attached_widgets();
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
            this->make_manager_active();
            this->draw_refresh_all_attached_widgets();
            break;

        default:
            this->notify_current_active_managed_model(_event);
            break;
        }
    }
}

void my_TestManager::notify_current_active_managed_model(UIControlEvent _event)
{
    my_IncrementalValueModel *current_active_model = (my_IncrementalValueModel *)this->managed_models[this->get_value()];
    TaskHandle_t handle = current_active_model->task_handle;
    xTaskNotify(handle, (uint32_t)_event, eSetValueWithOverwrite);
}
