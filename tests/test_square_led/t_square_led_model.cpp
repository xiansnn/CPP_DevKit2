/**
 * @file t_square_led_model.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "ui_core.h"
#include "ui_control_event.h"

/// @brief MySquareLedModel : Example of final implementation of ModelObject
class MySquareLedModel : public ModelObject
{
protected:
public:
    bool blinking_status = true;
    bool my_bool_value = true;
    MySquareLedModel();
    ~MySquareLedModel();
    void process_control_event(UIControlEvent _event);
};


MySquareLedModel::MySquareLedModel()
    : ModelObject()
{
}

MySquareLedModel::~MySquareLedModel()
{
}

void MySquareLedModel::process_control_event(UIControlEvent _event)
{
    switch (_event)
    {
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        my_bool_value = !my_bool_value;
        printf("on_off=%d\n", my_bool_value);
        set_change_flag();
        break;
    case UIControlEvent::LONG_PUSH:
        blinking_status = !blinking_status;
        printf("blink=%d\n", blinking_status);
        set_change_flag();
        break;

    default:
        break;
    }
}