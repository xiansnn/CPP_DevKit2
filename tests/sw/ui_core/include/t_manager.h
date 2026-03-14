#pragma once

#include "sw/ui_core/ui_core.h"

/// @brief implement a UIModelManager for test_ui_core program
class MyManager : public UIModelManager
{
private:
public:
    /// @brief Construct a new MyManager object
    /// @param _controller
    MyManager(UIController *_controller);
    ~MyManager();
    void process_control_event(struct_ControlEventData control_event);
};
