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
    : UIModelManager(is_wrappable), rtos_UIControlledModel()
{
}

my_TestManager::~my_TestManager()
{
}

void my_TestManager::process_control_event(UIControlEvent _event)
{
    TaskHandle_t handle;
    my_IncrementalValueModel *current_active_model;

    switch (_event)
    {
    case UIControlEvent::LONG_PUSH:
        if (this->current_active_model != this)
        {
            this->notify_current_active_model(_event);
        }
        break;
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (this->current_active_model == this)
        {
            this->make_managed_model_active();
            this->draw_refresh_all_attached_widgets();
            this->notify_current_active_model(_event);
        }
        else
        {
            this->make_manager_active();
            this->draw_refresh_all_attached_widgets();
        }
        break;
    case UIControlEvent::INCREMENT:
        if (this->current_active_model == this)
        {
            this->increment_focus();
            this->set_change_flag();
            this->draw_refresh_all_attached_widgets();
            // this->notify_all_linked_widget_task();
        }
        else
        {
            this->notify_current_active_model(_event);
        }
        break;
    case UIControlEvent::DECREMENT:
        if (this->current_active_model == this)
        {
            this->decrement_focus();
            this->set_change_flag();
            this->draw_refresh_all_attached_widgets();
            // this->notify_all_linked_widget_task();
        }
        else
        {
            this->notify_current_active_model(_event);
        }
        break;
    case UIControlEvent::TIME_OUT:
        break;
    default:
        break;
    }
}

void my_TestManager::notify_current_active_model(UIControlEvent _event)
{
    TaskHandle_t handle;
    my_IncrementalValueModel *current_active_model;
    current_active_model = (my_IncrementalValueModel *)this->managed_models[this->get_value()];
    handle = current_active_model->task_handle;
    xTaskNotify(handle, (uint32_t)_event, eSetValueWithOverwrite);
}
