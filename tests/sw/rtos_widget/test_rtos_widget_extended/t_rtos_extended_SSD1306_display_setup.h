#pragma once

#include "t_rtos_extended_SSD1306_display_setup.h"
#include "t_rtos_extended_config.h"

extern rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper;
extern rtos_HW_I2C_Master i2c_master;
extern struct_ConfigMasterI2C cfg_i2c;
extern struct_ConfigSSD1306 cfg_left_screen;
extern struct_ConfigSSD1306 cfg_right_screen;
extern struct_ConfigTextWidget SSD1306_title_config;
extern struct_ConfigTextWidget SSD1306_values_config;   
extern struct_ConfigGraphicWidget SSD1306_graph_config;

void I2C_display_gate_keeper_task(void *probe);
void i2c_irq_handler();