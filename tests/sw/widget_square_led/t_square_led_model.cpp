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

#include "sw/ui_core/ui_core.h"
#include "sw/ui_core/ui_control_event.h"

/// @brief MySquareLedModel : Example of final implementation of Model
class MySquareLedModel : public UIControlledModel
{
protected:

public:
    MySquareLedModel();
    ~MySquareLedModel();
    void process_control_event(UIControlEvent _event);
};

MySquareLedModel::MySquareLedModel()
    : UIControlledModel()
{
    set_change_flag();
}

MySquareLedModel::~MySquareLedModel()
{
}

void MySquareLedModel::process_control_event(UIControlEvent _event)
{
    switch (_event)
    {
    case UIControlEvent::RELEASED_AFTER_SHORT_TIME:
        switch (this->get_status())
        {
        case ControlledObjectStatus::HAS_FOCUS:
            update_status(ControlledObjectStatus::IS_WAITING);
            printf("Model : HAS_FOCUS -> IS_WAITING\n");
            break;
        case ControlledObjectStatus::IS_WAITING:
            update_status(ControlledObjectStatus::IS_ACTIVE);
            printf("Model : IS_WAITING -> IS_ACTIVE\n");
            break;
        case ControlledObjectStatus::IS_ACTIVE:
            update_status(ControlledObjectStatus::HAS_FOCUS);
            printf("Model : IS_ACTIVE -> HAS_FOCUS\n");
            break;
        default:
            break;
        }
    }
}