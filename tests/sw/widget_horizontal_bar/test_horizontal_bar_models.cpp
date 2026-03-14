/**
 * @file test_horizontal_bar.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "test_horizontal_bar_models.h"

MyControlledHorizontalBarModel::MyControlledHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, _increment)
{
}

void MyControlledHorizontalBarModel::process_control_event(struct_ControlEventData control_event)
{
    
    switch (control_event.event)
    {
    case UIControlEvent::INCREMENT:
        this->increment_value();
        break;
    case UIControlEvent::DECREMENT:
        this->decrement_value();
        break;
    default:
        break;
    }

}

MyControlledHorizontalBarModel::~MyControlledHorizontalBarModel()
{
}


MySimpleHorizontalBarModel::MySimpleHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment)
{
    this->min_value = _min_value;
    this->max_value = _max_value;
    this->is_wrappable = _is_wrappable;
    this->increment = _increment;
    this->value = min_value;
}

MySimpleHorizontalBarModel::~MySimpleHorizontalBarModel()
{
}

void MySimpleHorizontalBarModel::increment_value()
{
    value += increment;
    if (value > max_value)
        value = (is_wrappable) ? min_value : max_value;
    set_change_flag();
}

int MySimpleHorizontalBarModel::get_value()
{
    return value;
}
