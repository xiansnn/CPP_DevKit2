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

#include "sw/ui_core/rtos_ui_core.h"

class my_TestManager : public UIModelManager
{
private:
    /* data */
public:
    friend class rtos_my_TestManager;
    my_TestManager(bool is_wrappable = false);
    ~my_TestManager();
    void process_control_event(UIControlEvent _event);
};


/// @brief implement a UIModelManager for test_ui_core program
class rtos_my_TestManager : public rtos_UIModelManager
{
private:
public:
    /// @brief Construct a new MyManager object
    /// @param _controller
    rtos_my_TestManager(UIModelManager *linked_model_manager);
    ~rtos_my_TestManager();
    virtual void process_control_event_queue(struct_ControlEventData event_data) override;
};
