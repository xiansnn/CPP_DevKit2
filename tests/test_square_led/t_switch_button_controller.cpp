/**
 * @file t_switch_button_controller.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "device_switch_button/switch_button.h"
#include "sw_ui_core/ui_core.h"


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

MySwitchButton::MySwitchButton(uint gpio, 
                                struct_ConfigSwitchButton conf)
            : SwitchButton(gpio, conf), 
            UIController()
{
}

MySwitchButton::~MySwitchButton()
{
}
