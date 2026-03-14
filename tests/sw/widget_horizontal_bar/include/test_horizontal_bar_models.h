#pragma once
#include "sw/ui_core/ui_core.h"
// #include "sw/widget_horizontal_bar/widget_horizontal_bar.h"

class MySimpleHorizontalBarModel : public Model
{
private:
    int value;
    int min_value;
    int max_value;
    bool is_wrappable;
    int increment;

public:
    MySimpleHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment);
    ~MySimpleHorizontalBarModel();
    void increment_value();
    int get_value();
};


class MyControlledHorizontalBarModel : public UIControlledIncrementalValue
{
private:
    /* data */
public:
    MyControlledHorizontalBarModel(int _min_value, int _max_value, bool _is_wrappable, int _increment);
    ~MyControlledHorizontalBarModel();
    void process_control_event(struct_ControlEventData control_event);
};
