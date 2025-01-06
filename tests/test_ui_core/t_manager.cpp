/**
 * @file test_manager.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief companion file to test_ui_core.cpp
 * @version 0.1
 * @date 2024-05-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "t_manager.h"


test_Manager::test_Manager(UIController * _controller)
    : UIObjectManager()
{
    make_manager_active();
    update_current_controller(_controller);
}

/**
 * @brief Destroy the test_Manager::test_Manager object
 * 
 */
test_Manager::~test_Manager()
{
}

/**
 * @brief The event processed by test_Manager for this test are:
 * 
 * - LONG_PUSH: if manager is not active, processed by the current_active_model
 * 
 * - RELEASED_AFTER_SHORT_TIME: switch activation between the manager and the current model.
 * 
 * - INCREMENT: focus to next model
 * 
 * - DECREMENT: focus to previous model
 * 
 * @param _event 
 */
void test_Manager::process_control_event(ControlEvent _event)
{
    switch (_event)
    {
    case ControlEvent::NOOP:
        /* code */
        break;
    case ControlEvent::LONG_PUSH:
        if (current_active_model != this)
            current_active_model->process_control_event(_event);
        break;
    case ControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (current_active_model == this)
            make_managed_object_active();
        else
            make_manager_active();
        break;
    case ControlEvent::INCREMENT:
        if (current_active_model == this)
            increment_focus();
        else
            current_active_model->process_control_event(_event);
        break;
    case ControlEvent::DECREMENT:
        if (current_active_model == this)
            decrement_focus();
        else
            current_active_model->process_control_event(_event);
        break;
    default:
        break;
    }
}
