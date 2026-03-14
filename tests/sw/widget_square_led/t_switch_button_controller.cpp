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

 #include "t_switch_button_controller.h"

MySwitchButton::MySwitchButton(uint gpio, 
                                struct_ConfigSwitchButton conf)
            : SwitchButton(gpio, conf), 
            UIController()
{
}

MySwitchButton::~MySwitchButton()
{
}
