
#include "rtos_hw_i2c.h"

rtos_HW_I2C_Master::rtos_HW_I2C_Master(struct_ConfigMasterI2C cfg)
    : HW_I2C_Master(cfg)
{
    this->tx_dma = new HW_DMA();
    this->i2c_master_exclusive_irq_handler = cfg.i2c_tx_master_handler;
    i2c_tx_data_queue = xQueueCreate(8, sizeof(struct_TX_DataQueueI2C));
}

rtos_HW_I2C_Master::~rtos_HW_I2C_Master()
{
    delete tx_dma;
}

int rtos_HW_I2C_Master::burst_byte_write(uint8_t slave_address, uint8_t mem_address, uint8_t *source_address, size_t length)
{
    tx_dma->xfer_dma2i2c(this->i2c,
                         slave_address, mem_address, this->i2c_master_exclusive_irq_handler,
                         source_address, length);

    return 0;
}

int rtos_HW_I2C_Master::burst_byte_read(uint8_t slave_address,
                                        uint8_t mem_addr,
                                        uint16_t *destination_address,
                                        size_t length)
{
    tx_dma->xfer_i2c2dma(i2c, slave_address, mem_addr, this->i2c_master_exclusive_irq_handler, destination_address, length);
    return 0;
}

void rtos_HW_I2C_Master::i2c_dma_isr()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    irq_num_t irq_number = (this->i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    irq_set_enabled(irq_number, false); // disable IRQs to avoid re-entrance
    if (this->i2c->hw->intr_stat & I2C_IC_INTR_STAT_R_STOP_DET_BITS)
    {
        this->i2c->hw->clr_stop_det;       // clear the STOP_DET interrupt
        irq_set_enabled(irq_number, true); // disable IRQs to avoid re-entrance
        xSemaphoreGiveFromISR(this->tx_dma->end_of_xfer, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    if (this->i2c->hw->intr_stat & I2C_IC_INTR_STAT_R_TX_EMPTY_BITS)
    {
        xSemaphoreGiveFromISR(this->tx_dma->TX_FIFO_empty, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
