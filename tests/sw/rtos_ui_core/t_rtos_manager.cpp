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

// rtos_my_TestManager::rtos_my_TestManager(UIModelManager *linked_model_manager)
//     : rtos_UIModelManager(linked_model_manager)
// {
//     this->make_manager_active();

// }

// rtos_my_TestManager::~rtos_my_TestManager()
// {
// }

// /// @brief The event processed by void MyManager for this test are:
// /// - LONG_PUSH: if manager is not active, processed by the current_active_model
// /// - RELEASED_AFTER_SHORT_TIME: switch activation between the manager and the current model.
// /// - INCREMENT: focus to next model
// /// - DECREMENT: focus to previous model
// /// @param _event
// void rtos_my_TestManager::process_control_event_queue(struct_ControlEventData event_data)
// {
//     // my_TestManager *_my_model = (my_TestManager *)this->model;
//     UIControlEvent _event = event_data.event;
//     uint gpio = event_data.gpio_number;
//     switch (_event)
//     {
//     case UIControlEvent::NONE:
//         /* code */
//         break;
//     case UIControlEvent::LONG_PUSH:
//         if (this->current_active_model != this)
//         {
//             this->current_active_model->process_control_event(_event);
//         }
//         break;
//     case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
//         if (this->current_active_model == _my_model)
//         {
//             this->make_managed_model_active();
//             this->draw_refresh_all_attached_widgets();
//             this->current_active_model->draw_refresh_all_attached_widgets();
//         }
//         else
//         {
//             this->make_manager_active();
//             this->draw_refresh_all_attached_widgets();
//         }
//         break;
//     case UIControlEvent::INCREMENT:
//         if (this->current_active_model == _my_model)
//         {
//             this->increment_focus();
//             this->set_change_flag();
//             this->draw_refresh_all_attached_widgets();
//             // this->notify_all_linked_widget_task();
//         }
//         else
//         {
//             this->current_active_model->process_control_event(_event);
//         }
//         break;
//     case UIControlEvent::DECREMENT:
//         if (this->current_active_model == _my_model)
//         {
//             this->decrement_focus();
//             this->set_change_flag();
//             this->draw_refresh_all_attached_widgets();
//             // this->notify_all_linked_widget_task();
//         }
//         else
//         {

//             this->current_active_model->process_control_event(_event);
//         }
//         break;
//     case UIControlEvent::TIME_OUT:
//         break;
//     default:
//         break;
//     }
// }

my_TestManager::my_TestManager(bool is_wrappable)
    : UIModelManager(is_wrappable), rtos_UIModelManager()
{
}

my_TestManager::~my_TestManager()
{
}

void my_TestManager::process_control_event(UIControlEvent _event)
{
}

void my_TestManager::process_control_event_queue(struct_ControlEventData event_data)
{
    // my_TestManager *_my_model = (my_TestManager *)this->model;
    UIControlEvent _event = event_data.event;
    // uint gpio = event_data.gpio_number;
    switch (_event)
    {
    case UIControlEvent::NONE:
        /* code */
        break;
    case UIControlEvent::LONG_PUSH:
        if (this->current_active_model != this)
        {
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
