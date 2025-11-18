/**
 * @file t_rtos_controlled_value.h
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

/// @brief Construct an implementation of UIControlledIncrementalValue for test_ui_core program.
class my_IncrementalValueModel : public UIControlledIncrementalValue, public rtos_UIControlledModel
{
private:
public:
    std::string name;
    my_IncrementalValueModel(std::string _name,
                            int _min_value = 0,
                            int _max_value = 10,
                            bool _is_wrappable = false,
                            int increment = 1);
    ~my_IncrementalValueModel();
    void process_control_event(struct_ControlEventData control_event);
    std::string get_name();
};

