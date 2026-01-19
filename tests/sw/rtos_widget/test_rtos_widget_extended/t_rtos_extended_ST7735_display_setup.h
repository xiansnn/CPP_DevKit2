#pragma once

#include "t_rtos_extended_config.h"


extern rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper ;
extern rtos_HW_SPI_Master spi_master;
extern struct_ConfigMasterSPI cfg_spi;
extern struct_ConfigST7735 cfg_st7735;
extern struct_ConfigTextWidget ST7735_title_config;
extern struct_ConfigTextWidget ST7735_values_config;    
extern struct_ConfigGraphicWidget ST7735_graph_config;

void end_of_TX_DMA_xfer_handler();
void SPI_display_gate_keeper_task(void *probe);