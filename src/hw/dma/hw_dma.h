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

/// @brief Configuration structure for DMA
struct struct_ConfigDMA
{
    /// @brief width of DMA transfers words, can be DMA_SIZE_8, DMA_SIZE_16, DMA_SIZE_32
    dma_channel_transfer_size transfer_size = DMA_SIZE_8;
    /// @brief number of words to be transfered in each burst
    uint number_of_transfer;
    /// @brief the pointer to the handler for DMA IRQ, can be NULL if no IRQ is needed
    irq_handler_t dma_irq_handler = NULL;
    /// @brief the IRQ number for the DMA channel, can be DMA_IRQ_0 or DMA_IRQ_1
    irq_num_t irq_number;
};

/**
 * @brief Class to manage a DMA channel
 *
 */
class HW_DMA
{
private:
    /// @brief
    dma_channel_config c;

public:
    /// @brief the IRQ number for the DMA channel, can be DMA_IRQ_0 or DMA_IRQ_1
    irq_num_t irq_number;
    /// @brief the DMA channel number allocated
    uint channel;
    /// @brief semaphore to signal the end of a DMA transfer
    SemaphoreHandle_t end_of_xfer;
    /// @brief semaphore to signal the TX FIFO of I2C is empty
    SemaphoreHandle_t TX_FIFO_empty; // TODO move to I2C

    /// @brief Constructor of the class
    /// @param channel a specific DMA channel number to allocate
    /// @param cfg  the DMA configuration data structure
    HW_DMA(struct_ConfigDMA cfg);

    /// @brief Destructor of the class, free the allocated DMA channel
    ~HW_DMA();

    /// @brief the function member used to move data from memory to memory
    /// @param cfg the DMA configuration data structure
    /// @param destination_address the destination address
    /// @param source_address the source address
    /// @param start if true, start the DMA transfer immediately
    /// @return error code
    int xfer_mem2mem(struct_ConfigDMA *cfg,
                     volatile void *destination_address,
                     volatile void *source_address,
                     bool start);

    /// @brief the function member used to move data from memory to I2C peripheral
    /// @param i2c the I2C instance, can be i2c0 or i2c1
    /// @param slave_address the 7-bit I2C slave address
    /// @param slave_mem_addr the memory address in the slave device to write to
    /// @param i2c_handler the IRQ handler for I2C, can be NULL if no IRQ is needed
    /// @param source_address the source address
    /// @param length the number of bytes to write
    /// @param start if true, start the DMA transfer immediately
    /// @return error code
    int xfer_dma2i2c(i2c_inst_t *i2c,
                     uint8_t slave_address,
                     uint8_t slave_mem_addr,
                     irq_handler_t i2c_handler,
                     volatile uint8_t *source_address,
                     size_t length,
                     bool start = true); // TODO move to I2C
    /// @brief the function member used to move data from I2C peripheral to memory
    /// @param i2c  the I2C instance, can be i2c0 or i2c1
    /// @param slave_address    the 7-bit I2C slave address
    /// @param slave_mem_addr   the memory address in the slave device to read from
    /// @param i2c_handler  the IRQ handler for I2C, can be NULL if no IRQ is needed
    /// @param destination_address     the destination address
    /// @param length   the number of bytes to read
    /// @param start    if true, start the DMA transfer immediately
    /// @return error code
    int xfer_i2c2dma(i2c_inst_t *i2c,
                     uint8_t slave_address,
                     uint8_t slave_mem_addr,
                     irq_handler_t i2c_handler,
                     volatile uint16_t *destination_address,
                     size_t length,
                     bool start = true); // TODO move to I2C

    /// @brief clean up and free the allocated DMA channel
    void cleanup_and_free_dma_channel();

    /// @brief start the DMA transfer
    void start_dma();
};
