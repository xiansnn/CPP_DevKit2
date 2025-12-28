#pragma once

#include "t_rtos_extended_SSD1306_display_setup.h"
#include "t_rtos_extended_config.h"

extern rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper;
extern rtos_HW_I2C_Master i2c_master;


void I2C_display_gate_keeper_task(void *probe);
void i2c_irq_handler();