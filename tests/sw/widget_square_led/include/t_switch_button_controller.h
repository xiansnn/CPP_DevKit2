#pragma once

#include "sw/ui_core/ui_core.h"
#include "device/switch_button/switch_button.h"


/**
 * @brief test_switch_button : Example of final implementation of SwitchButton and UIController
 *
 */
class MySwitchButton : public SwitchButton, public UIController
{
private:
    /* data */
public:
    MySwitchButton(uint gpio, struct_ConfigSwitchButton conf);
    ~MySwitchButton();
};
