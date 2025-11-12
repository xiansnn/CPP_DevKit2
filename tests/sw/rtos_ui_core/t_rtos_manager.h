/**
 * @file t_rtos_manager.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include "sw/ui_core/rtos_ui_core.h"

class my_TestManager : public UIModelManager, public rtos_UIModelManager
{
private:
public:
    my_TestManager(bool is_wrappable = false);
    ~my_TestManager();
    void process_control_event(UIControlEvent _event);
    void process_control_event_queue(struct_ControlEventData event_data);
};
