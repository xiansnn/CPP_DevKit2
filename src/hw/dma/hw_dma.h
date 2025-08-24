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
    uint channel;
    // dma_channel_transfer_size transfer_size = DMA_SIZE_8; // can be DMA_SIZE_8, DMA_SIZE_16, DMA_SIZE_32
    // uint block_size;                                      // number of transfer to be executed
    // irq_handler_t handler = NULL;
    // irq_num_t irq_number; // can be DMQ_IRQ_0 or DMA_IRQ_1

public:
    HW_DMA();
    HW_DMA(uint channel, struct_ConfigDMA *cfg);
    ~HW_DMA();

    void write_mem2mem(struct_ConfigDMA *cfg, volatile void *write_address, volatile void *read_address, bool start);
    void write_dma2spi(struct_ConfigDMA *dma_cfg, struct_ConfigMasterSPI *spi_cfg, volatile void *read_address, bool start);
    void write_spi2dma(struct_ConfigMasterSPI *spi_cfg, struct_ConfigDMA *dma_cfg, volatile void *write_address, bool start);
    void cleanup_and_free_dma_channel();
    void start_dma();

};
