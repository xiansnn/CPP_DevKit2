/**
 * @file test_controlled_value.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief companion file to test_ui_core.cpp
 * @version 0.1
 * @date 2024-05-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "t_controlled_value.h"

test_IncrementalValue::test_IncrementalValue(std::string _name, int _min_value, int _max_value, bool _is_wrappable, int increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, increment)
{
    this->name = _name;
}

test_IncrementalValue::~test_IncrementalValue()
{
}

/**
 * @brief The event processed by test_IncrementalValue for this test are:
 * 
 * - LONG_PUSH: set value to 0.
 * 
 * - INCREMENT:
 * 
 * - DECREMENT:
 * 
 * @param _event 
 */
void test_IncrementalValue::process_control_event(ControlEvent _event)
{
    switch (_event)
    {
    case ControlEvent::LONG_PUSH:
        this->set_clipped_value(0);
        break;
    case ControlEvent::INCREMENT:
        increment_value();
        break;
    case ControlEvent::DECREMENT:
        decrement_value();
        break;
    default:
        printf("test_IncrementalValue::process_control_event(ControlEvent _event) = default\n");
        break;
    }
}

std::string test_IncrementalValue::get_name()
{
    return name;
}
