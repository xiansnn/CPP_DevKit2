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

#pragma once

#include "ui_control_event.h"
#include "ui_core.h"
// #define PRINT_DEBUG

/**
 * @brief MyManagedSquareLedModel : Example of final implementation of ModelObject
 */
class MyManagedSquareLedModel : public Model
{
protected:
public:
    /// @brief gives a name to the model. Useful to identify the active model via printf on the serial monitor.
    std::string name;
    /// @brief a bool value, visualised by the associated LED widget as on or off.
    bool my_bool_value = false;
    MyManagedSquareLedModel(std::string name);
    ~MyManagedSquareLedModel();
    /// @brief the implementation of the pure virtual member.
    /// @param _event the signal send by the controller
    void process_control_event(UIControlEvent _event);
};
MyManagedSquareLedModel::MyManagedSquareLedModel(std::string _name)
    : Model()
{
    this->name = _name;
}

MyManagedSquareLedModel::~MyManagedSquareLedModel()
{
}

void MyManagedSquareLedModel::process_control_event(UIControlEvent _event)
{
    /**
     * @brief only the event INCREMENT and DECREMENT are processed.
     * They both toggle the boolean my_bool_value of the test_managed_square_led_model, and set the change flag in order to trig the effective display.
     */
    switch (_event)
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
