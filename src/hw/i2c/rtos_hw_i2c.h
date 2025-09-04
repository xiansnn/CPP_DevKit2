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

#define BURST_SIZE 16

class rtos_HW_I2C_Master : public HW_I2C_Master
{
private:
    HW_DMA *tx_dma;
public:
    rtos_HW_I2C_Master(struct_ConfigMasterI2C cfg);
    ~rtos_HW_I2C_Master();
    
    /// @brief this the FreeRTOS task handle attached to the task that uses this I2C Master burst_byte_write function member
    TaskHandle_t task_to_notify_when_fifo_empty = NULL;

    void burst_byte_write(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *src, size_t len);
    void i2c_tx_fifo_dma_isr();
};
