/**
 * @file t_managed_horizontal_bar_models.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-13
 *
 * @copyright Copyright (c) 2025
 *
 */

 #include "t_managed_horizontal_bar_models.h"

// #define PRINT_DEBUG

MyControlledHorizontalBarModel::MyControlledHorizontalBarModel(std::string _name,
                                           int _min_value, int _max_value,
                                           bool _is_wrappable, int _increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, _increment)
{
    this->name = _name;
}

MyControlledHorizontalBarModel::~MyControlledHorizontalBarModel()
{
}

void MyControlledHorizontalBarModel::process_control_event(struct_ControlEventData   control_event)
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
#ifdef PRINT_DEBUG
    printf("---> %s value=%d\n", this->name.c_str(), this->value);
#endif
}
