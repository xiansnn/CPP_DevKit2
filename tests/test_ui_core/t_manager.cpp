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
#pragma once

#include "ui_core.h"

/// @brief implement a UIObjectManager for test_ui_core program
class MyManager : public UIObjectManager
{
private:
public:
    /// @brief Construct a new MyManager object
    /// @param _controller
    MyManager(UIController *_controller);
    ~MyManager();
    void process_control_event(UIControlEvent _event);
};

MyManager::MyManager(UIController *_controller)
    : UIObjectManager()
{
    make_manager_active();
    update_current_controller(_controller);
}

MyManager::~MyManager()
{
}

/// @brief The event processed by void MyManager for this test are:
/// - LONG_PUSH: if manager is not active, processed by the current_active_model
/// - RELEASED_AFTER_SHORT_TIME: switch activation between the manager and the current model.
/// - INCREMENT: focus to next model
/// - DECREMENT: focus to previous model
/// @param _event
void MyManager::process_control_event(UIControlEvent _event) 
{
    switch (_event)
    {
    case UIControlEvent::NONE:
        /* code */
        break;
    case UIControlEvent::LONG_PUSH:
        if (current_active_model != this)
            current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        if (current_active_model == this)
            make_managed_object_active();
        else
            make_manager_active();
        break;
    case UIControlEvent::INCREMENT:
        if (current_active_model == this)
            {increment_focus();
            this->set_change_flag();}
        else
            current_active_model->process_control_event(_event);
        break;
    case UIControlEvent::DECREMENT:
        if (current_active_model == this)
            {decrement_focus();
            this->set_change_flag();}
        else
            current_active_model->process_control_event(_event);
        break;
    default:
        break;
    }
}
