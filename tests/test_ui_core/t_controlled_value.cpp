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
#include "ui_core.h"

/// @brief Construct an implementation of UIControlledIncrementalValue for test_ui_core program.
class MyIncrementalValueModel : public UIControlledIncrementalValue
{
private:
public:
    std::string name;
    MyIncrementalValueModel(std::string _name,
                            int _min_value = 0,
                            int _max_value = 10,
                            bool _is_wrappable = false,
                            int increment = 1);
    ~MyIncrementalValueModel();
    void process_control_event(UIControlEvent _event);
    std::string get_name();
};

MyIncrementalValueModel::MyIncrementalValueModel(std::string _name,
                                                 int _min_value,
                                                 int _max_value,
                                                 bool _is_wrappable,
                                                 int increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, increment)
{
    this->name = _name;
}

MyIncrementalValueModel::~MyIncrementalValueModel()
{
}

/// @brief The event processed by test_IncrementalValue for this test are:
/// - LONG_PUSH: set value to 0.
/// - INCREMENT:
/// - DECREMENT:
/// @param _event
void MyIncrementalValueModel::process_control_event(UIControlEvent _event)
{
    switch (_event)
    {
    case UIControlEvent::LONG_PUSH:
        this->set_clipped_value(0);
        break;
    case UIControlEvent::INCREMENT:
        increment_value();
        break;
    case UIControlEvent::DECREMENT:
        decrement_value();
        break;
    default:
        printf("test_IncrementalValue::process_control_event(ControlEvent _event) = default\n");
        break;
    }
}

std::string MyIncrementalValueModel::get_name()
{
    return name;
}
