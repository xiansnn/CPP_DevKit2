/**
 * @file t_rtos_all_device_roll_control.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-19
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "sw/ui_core/rtos_ui_core.h"
#include "t_rtos_all_device_defines.h"

/// @brief Construct an implementation of UIControlledIncrementalValue for test_ui_core program.
class my_ControlledRollPosition : public rtos_UIControlledModel
{
private:
public:
    std::string name;
    my_ControlledRollPosition(std::string name);
    ~my_ControlledRollPosition();
    void process_control_event(struct_ControlEventData control_event);
};

class my_PositionController : public rtos_UIModelManager
{
private:
public:
    my_PositionController(bool is_wrapable = false);
    ~my_PositionController();
    void process_control_event(struct_ControlEventData control_event);
};
