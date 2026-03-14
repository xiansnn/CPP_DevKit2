#pragma once
#include "sw/ui_core/ui_core.h"
#include "sw/ui_core/ui_control_event.h"


/// @brief MySquareLedModel : Example of final implementation of Model
class MySquareLedModel : public UIControlledModel
{
protected:

public:
    MySquareLedModel();
    ~MySquareLedModel();
    void process_control_event(struct_ControlEventData control_event);
};
