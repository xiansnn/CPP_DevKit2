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

my_TestManager::my_TestManager(bool is_wrappable)
    : UIModelManager(is_wrappable), rtos_UIControlledModel()
{
}

my_TestManager::~my_TestManager()
{
}

void my_TestManager::process_control_event(UIControlEvent _event)
{

    switch (_event)
    {
    case UIControlEvent::NONE:
        /* code */
        break;
    case UIControlEvent::LONG_PUSH:
        if (this->current_active_model != this)
        {
            // TaskHandle_t current_active_model_task_handle = ((rtos_UIControlledModel *)this->current_active_model)->task_handle;
            // printf("LONG_PUSH my_TestManager::process_control_event.current_active_model_task_handle : %p\n", current_active_model_task_handle);
            // xTaskNotify(current_active_model_task_handle, (uint32_t)UIControlEvent::LONG_PUSH, eSetValueWithOverwrite);
            this->current_active_model->process_control_event(_event);
        }
        break;
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (this->current_active_model == this)
        {
            this->make_managed_model_active();
            this->draw_refresh_all_attached_widgets();
            this->current_active_model->draw_refresh_all_attached_widgets();
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
            this->current_active_model->process_control_event(_event);
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

            this->current_active_model->process_control_event(_event);
        }
        break;
    case UIControlEvent::TIME_OUT:
        break;
    default:
        break;
    }
}