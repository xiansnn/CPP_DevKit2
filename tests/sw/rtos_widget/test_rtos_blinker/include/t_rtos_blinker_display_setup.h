#pragma once

#include "t_rtos_blinker_config.h"

#include "device/ST7735/st7735.h"
#include "device/SSD1306/ssd1306.h"


extern rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper ;
extern rtos_HW_SPI_Master spi_master;
extern struct_ConfigMasterSPI cfg_spi;
extern struct_ConfigST7735 cfg_st7735;


void end_of_TX_DMA_xfer_handler();
void SPI_display_gate_keeper_task(void *probe);


extern rtos_GraphicDisplayGateKeeper I2C_display_gate_keeper;
extern rtos_HW_I2C_Master i2c_master;
extern struct_ConfigMasterI2C cfg_i2c;
extern struct_ConfigSSD1306 cfg_left_screen;
extern struct_ConfigSSD1306 cfg_right_screen;

void I2C_display_gate_keeper_task(void *probe);
void i2c_irq_handler();

