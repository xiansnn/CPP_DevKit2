#include "t_controlled_value.h"

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
void MyIncrementalValueModel::process_control_event(struct_ControlEventData control_event)
{
    switch (control_event.event)
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
        break;
    }
}

std::string MyIncrementalValueModel::get_name()
{
    return name;
}
