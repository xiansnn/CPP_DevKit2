#pragma once

#include "t_rtos_extended_config.h"


extern rtos_GraphicDisplayGateKeeper SPI_display_gate_keeper ;
extern rtos_HW_SPI_Master spi_master;

void end_of_TX_DMA_xfer_handler();
void SPI_display_gate_keeper_task(void *probe);