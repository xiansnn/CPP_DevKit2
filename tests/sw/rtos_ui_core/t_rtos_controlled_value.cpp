/**
 * @file t_rtos_controlled_value.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-11-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "t_rtos_controlled_value.h"

my_IncrementalValueModel::my_IncrementalValueModel(std::string _name,
                                                   int _min_value,
                                                   int _max_value,
                                                   bool _is_wrappable,
                                                   int increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, increment)
{
    this->name = _name;
};

my_IncrementalValueModel::~my_IncrementalValueModel() {
};

/// @brief The event processed by test_IncrementalValue for this test are:
/// - LONG_PUSH: set value to 0.
/// - INCREMENT:
/// - DECREMENT:
/// @param _event
void my_IncrementalValueModel::process_control_event(UIControlEvent _event)
{
    switch (_event)
    {
    case UIControlEvent::LONG_PUSH:
        this->set_clipped_value(0);
        this->draw_refresh_all_attached_widgets();
        break;
        case UIControlEvent::INCREMENT:
        increment_value();
        this->draw_refresh_all_attached_widgets();
        break;
        case UIControlEvent::DECREMENT:
        decrement_value();
        this->draw_refresh_all_attached_widgets();
        break;
    default:
        break;
    }
}

std::string my_IncrementalValueModel::get_name()
{
    return name;
};
