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

#pragma once

#include "ui_control_event.h"
#include "ui_core.h"

#define PRINT_DEBUG

class MyHorizontalBarModel : public UIControlledIncrementalValue
{
private:
public:
    MyHorizontalBarModel(std::string _name, int _min_value, int _max_value, bool _is_wrappable, int _increment);
    ~MyHorizontalBarModel();
    std::string name;
    void process_control_event(UIControlEvent _event);
};

MyHorizontalBarModel::MyHorizontalBarModel(std::string _name, int _min_value, int _max_value, bool _is_wrappable, int _increment)
    : UIControlledIncrementalValue(_min_value, _max_value, _is_wrappable, _increment)
{
    this->name = _name;
}

MyHorizontalBarModel::~MyHorizontalBarModel()
{
}

void MyHorizontalBarModel::process_control_event(UIControlEvent _event)
{
    switch (_event)
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
