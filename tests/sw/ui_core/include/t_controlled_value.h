#pragma once

#include "sw/ui_core/ui_core.h"

/// @brief Construct an implementation of UIControlledIncrementalValue for test_ui_core program.
class MyIncrementalValueModel : public UIControlledIncrementalValue
{
private:
public:
    std::string name;
    MyIncrementalValueModel(std::string _name,
                            int _min_value = 0,
                            int _max_value = 10,
                            bool _is_wrappable = false,
                            int increment = 1);
    ~MyIncrementalValueModel();
    void process_control_event(struct_ControlEventData control_event);
    std::string get_name();
};