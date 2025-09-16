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

/// @brief I2C max FIFO size (according to RP2040 datasheet)
#define I2C_BURST_SIZE 16


/**
 * @brief Class to manage a DMA channel
 *
 */
class HW_DMA
{
private:

public:
    /// @brief the IRQ number for the DMA channel, can be DMA_IRQ_0 or DMA_IRQ_1
    irq_num_t irq_number;
    /// @brief the DMA channel number allocated
    uint channel;
    /// @brief semaphore to signal the end of a DMA transfer
    SemaphoreHandle_t end_of_xfer;

    /// @brief  constructor of the DMA class
    /// @param irq_number the IRQ attached to this DMA object
    /// @param dma_irq_handler the IRQ hansler triggered by this IRQ
    HW_DMA(irq_num_t irq_number = DMA_IRQ_0, irq_handler_t dma_irq_handler = NULL);

    /// @brief Destructor of the class, free the allocated DMA channel
    ~HW_DMA();

    /// @brief the function member used to move data from memory to memory
    /// @param number_of_transfer the number of DMA transfers
    /// @param transfer_size the size of DMA transfer (8, 16 or 32 bit)
    /// @param destination_address the memory destination address
    /// @param source_address the memory source address
    /// @param start if trus, immediate start
    /// @return error code
    int xfer_mem2mem(uint32_t number_of_transfer,
                     dma_channel_transfer_size_t transfer_size,
                     volatile void *destination_address,
                     volatile void *source_address,
                     bool start);

    /// @brief clean up and free the allocated DMA channel
    void cleanup_and_free_dma_channel();

    /// @brief start the DMA transfer
    void start_dma();
};
