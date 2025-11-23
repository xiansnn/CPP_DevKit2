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
    : core_IncrementControlledModel(_min_value, _max_value, _is_wrappable, increment)
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
void my_IncrementalValueModel::process_control_event(struct_ControlEventData control_event)
{
    bool changed;
    UIControlEvent _event = control_event.event;
    switch (_event)
    {
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        this->notify_all_linked_widget_task();
        break;
    case UIControlEvent::LONG_PUSH:

        changed = this->set_clipped_value(0);
        if (changed)
            this->notify_all_linked_widget_task();
        break;
    case UIControlEvent::INCREMENT:
        changed = this->increment_value();
        if (changed)
            this->notify_all_linked_widget_task();
        break;
    case UIControlEvent::DECREMENT:
        changed = this->decrement_value();
        if (changed)
            this->notify_all_linked_widget_task();
        break;
    case UIControlEvent::TIME_OUT:
    default:
        break;
    }
}

std::string my_IncrementalValueModel::get_name()
{
    return name;
};
