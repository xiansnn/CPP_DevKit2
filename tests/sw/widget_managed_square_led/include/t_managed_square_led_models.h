#pragma once

#include "sw/ui_core/ui_control_event.h"    
#include "sw/ui_core/ui_core.h"


/**
 * @brief MyManagedSquareLedModel : Example of final implementation of ModelObject
 */
class MyManagedSquareLedModel : public UIControlledModel
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
    void process_control_event(struct_ControlEventData control_event);
};