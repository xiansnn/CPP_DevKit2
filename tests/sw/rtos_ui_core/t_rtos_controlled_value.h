/**
 * @file t_controlled_value.cpp
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

/// @brief Construct an implementation of UIControlledIncrementalValue for test_ui_core program.
class my_IncrementalValueModel : public UIControlledIncrementalValue
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
    void process_control_event(UIControlEvent _event);
    std::string get_name();
};

class rtos_my_IncrementalValueModel : public rtos_UIControlledIncrementalValue
{
private:
    /* data */
public:
    rtos_my_IncrementalValueModel(my_IncrementalValueModel *linked_ui_controlled_incremental_value);
    ~rtos_my_IncrementalValueModel();
    void process_control_event_queue(struct_ControlEventData event_data);
};

