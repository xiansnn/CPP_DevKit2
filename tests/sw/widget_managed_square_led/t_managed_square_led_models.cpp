/**
 * @file t_managed_square_led_models.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-12
 *
 * @copyright Copyright (c) 2025
 *
 */


#include "t_managed_square_led_models.h"

#define PRINT_DEBUG

MyManagedSquareLedModel::MyManagedSquareLedModel(std::string _name)
    : UIControlledModel()
{
    this->name = _name;
}

MyManagedSquareLedModel::~MyManagedSquareLedModel()
{
}

void MyManagedSquareLedModel::process_control_event(struct_ControlEventData control_event)
{
    /**
     * @brief only the event INCREMENT and DECREMENT are processed.
     * They both toggle the boolean my_bool_value of the test_managed_square_led_model, and set the change flag in order to trig the effective display.
     */
    switch (control_event.event)
    {
    case UIControlEvent::INCREMENT:
    case UIControlEvent::DECREMENT:
        if (get_status() == ControlledObjectStatus::IS_ACTIVE)
        {
            my_bool_value = !my_bool_value;
            set_change_flag();
#ifdef PRINT_DEBUG
            printf("---> %s on_off=%d\n", this->name.c_str(), my_bool_value);
#endif
        }
        break;

    default:
        break;
    }
}
