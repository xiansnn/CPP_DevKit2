/**
 * @file t_manager.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "t_rtos_manager.h"

rtos_my_TestManager::rtos_my_TestManager(UIModelManager *linked_model_manager)
    : rtos_UIModelManager(linked_model_manager)
{
    ((my_TestManager *)this->model)->make_manager_active();
    // update_current_controller(_controller);
}

rtos_my_TestManager::~rtos_my_TestManager()
{
}

/// @brief The event processed by void MyManager for this test are:
/// - LONG_PUSH: if manager is not active, processed by the current_active_model
/// - RELEASED_AFTER_SHORT_TIME: switch activation between the manager and the current model.
/// - INCREMENT: focus to next model
/// - DECREMENT: focus to previous model
/// @param _event
void rtos_my_TestManager::process_control_event_queue(struct_ControlEventData event_data)
{
    my_TestManager *_my_model = (my_TestManager *)this->model;
    UIControlEvent _event = event_data.event;
    switch (_event)
    {
    case UIControlEvent::NONE:
        /* code */
        break;
    case UIControlEvent::LONG_PUSH:
        printf("rtos_my_TestManager::process_control_event_queue->LONG_PUSH\n");
        if (_my_model->current_active_model != _my_model)
            _my_model->current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        printf("rtos_my_TestManager::process_control_event_queue->RELEASED_AFTER_SHORT_TIME\n");
        if (_my_model->current_active_model == _my_model)
            _my_model->make_managed_model_active();
        else
            _my_model->make_manager_active();
        break;
    case UIControlEvent::INCREMENT:
        printf("rtos_my_TestManager::process_control_event_queue->INCREMENT\n");
        if (_my_model->current_active_model == _my_model)
        {
            _my_model->increment_focus();
            _my_model->set_change_flag();
            this->notify_all_linked_widget_task();
        }
        else
            _my_model->current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::DECREMENT:
        printf("rtos_my_TestManager::process_control_event_queue->DECREMENT\n");
        if (_my_model->current_active_model == _my_model)
        {
            _my_model->decrement_focus();
            this->notify_all_linked_widget_task();
            // this->set_change_flag();
        }
        else
            _my_model->current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::TIME_OUT:
        printf("rtos_my_TestManager::process_control_event_queue->TIME_OUT\n");
        break;
    case UIControlEvent::RELEASED_AFTER_LONG_TIME:
        printf("rtos_my_TestManager::process_control_event_queue->RELEASED_AFTER_LONG_TIME\n");
        break;
    case UIControlEvent::PUSH:
        printf("rtos_my_TestManager::process_control_event_queue->PUSH\n");
        break;
    case UIControlEvent::DOUBLE_PUSH:
        printf("rtos_my_TestManager::process_control_event_queue->DOUBLE_PUSH\n");
        break;
    default:
        printf("rtos_my_TestManager::process_control_event_queue->DEFAULT\n");
        break;
    }
}

my_TestManager::my_TestManager(bool is_wrappable)
    : UIModelManager(is_wrappable)
{
}

my_TestManager::~my_TestManager()
{
}

void my_TestManager::process_control_event(UIControlEvent _event)
{
}
