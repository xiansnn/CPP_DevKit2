/**
 * @file hw_dma.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-24
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hw/spi/hw_spi.h"
#include "hw/i2c/hw_i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define I2C_BURST_SIZE 16

struct struct_ConfigDMA
{
    dma_channel_transfer_size transfer_size = DMA_SIZE_8; // can be DMA_SIZE_8, DMA_SIZE_16, DMA_SIZE_32
    uint block_size;                                      // number of transfer to be executed
    irq_handler_t handler = NULL;
    irq_num_t irq_number; // can be DMQ_IRQ_0 or DMA_IRQ_1
};

class HW_DMA
{
private:
    dma_channel_config c;
    irq_num_t irq_number;

public:
    uint channel;
    SemaphoreHandle_t end_of_xfer = xSemaphoreCreateBinary();
    SemaphoreHandle_t TX_FIFO_empty = xSemaphoreCreateBinary();

    HW_DMA();
    HW_DMA(uint channel, struct_ConfigDMA *cfg);
    ~HW_DMA();

    void xfer_mem2mem(struct_ConfigDMA *cfg, volatile void *write_address, volatile void *read_address, bool start);
    void xfer_dma2spi(struct_ConfigDMA *dma_cfg, struct_ConfigMasterSPI *spi_cfg, volatile void *read_address, bool start);
    void xfer_spi2dma(struct_ConfigMasterSPI *spi_cfg, struct_ConfigDMA *dma_cfg, volatile void *write_address, bool start);
    void xfer_dma2i2c(i2c_inst_t *i2c, uint8_t slave_address, uint8_t slave_mem_addr, irq_handler_t i2c_handler,
                      volatile uint8_t *read_address, size_t length, bool start = true);
    void xfer_i2c2dma(i2c_inst_t *i2c, uint8_t slave_address, uint8_t slave_mem_addr, irq_handler_t i2c_handler,
                      volatile uint16_t *read_address, size_t length, bool start = true);
    void cleanup_and_free_dma_channel();
    void start_dma();
};
