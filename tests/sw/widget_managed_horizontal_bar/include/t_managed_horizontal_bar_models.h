#pragma once

#include "sw/ui_core/ui_control_event.h"
#include "sw/ui_core/ui_core.h"

class MyControlledHorizontalBarModel : public UIControlledIncrementalValue
{
private:
public:
    MyControlledHorizontalBarModel(std::string _name, int _min_value, int _max_value, bool _is_wrappable, int _increment);
    ~MyControlledHorizontalBarModel();
    std::string name;
    void process_control_event(struct_ControlEventData control_event);
};
