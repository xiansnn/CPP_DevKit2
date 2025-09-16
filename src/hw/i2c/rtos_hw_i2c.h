/**
 * @file rtos_hw_i2c.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-02
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "hw_i2c.h"
#include "hw/dma/hw_dma.h"
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/irq.h"

/// @brief I2C max FIFO size (according to RP2040 datasheet)
#define BURST_SIZE 16

/// @brief Data structure for I2C TX queue
struct struct_TX_DataQueueI2C
{
    /// @brief pointer to the data buffer to send
    uint8_t *write_data;

    /// @brief memory address in the slave device to write to
    uint8_t mem_address;

    /// @brief number of bytes to write
    uint8_t write_data_length;
};
/// @brief Data structure for I2C RX queue
struct struct_RX_DataQueueI2C
{
    /// @brief memory address in the slave device to read from
    uint8_t mem_address;

    /// @brief the size of the data to read
    uint8_t read_data_length;
};

/// @brief This is a C++ wrapper for the original pico SDK i2c master API, adapted to FreeRTOS environment
/// \ingroup hw
class rtos_HW_I2C_Master : public HW_I2C_Master
{
private:
public:
    /// @brief the DMA channel used for I2C TX
    HW_DMA *tx_dma;

    /// @brief the freeRTOS queue used to exchange data from a generator to the I2C TX task
    QueueHandle_t i2c_tx_data_queue;

    /// @brief semaphore to signal the TX FIFO of I2C is empty
    SemaphoreHandle_t i2c_tx_FIFO_empty; 


    /// @brief  Constructor of the class
    /// @param cfg  the configuration structure for the I2C master
    rtos_HW_I2C_Master(struct_ConfigMasterI2C cfg);

    /// @brief Destructor of the class
    ~rtos_HW_I2C_Master();


    /// @brief a function member to write a burst of data to a slave device, using DMA and FreeRTOS queue
    /// @param slave_address the 7-bit I2C slave address
    /// @param mem_addr the adresse to write in the slave peripheral
    /// @param source_address the address of the data to send
    /// @param length the number of data to send
    /// @return code error
    int burst_byte_write(uint8_t slave_address,
                         uint8_t mem_addr,
                         uint8_t *source_address,
                         size_t length);


    /**
     * @brief a convenient C++ member wrapper to read a block of data starting at a slave memory address, using DMA and FreeRTOS queue
     * @param slave_address the slave address
     * @param mem_addr the starting address of slave memory to read from
     * @param destination_address Pointer to buffer to receive data
     * @param length the size of the block of data
     * @return code error
     */
    int burst_byte_read(uint8_t slave_address,
                        uint8_t mem_addr, 
                        uint16_t *destination_address,
                        size_t length);

    /// @brief the I2C IRQ handler, to be called in the main program IRQ map
    void i2c_dma_isr();
};
