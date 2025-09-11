
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
}

void rtos_HW_I2C_Master::burst_byte_write(uint8_t slave_address, struct_TX_DataQueueI2C data_to_send)
{
    tx_dma->xfer_dma2i2c(this->i2c, slave_address, data_to_send.mem_address, this->i2c_master_exclusive_irq_handler, data_to_send.write_data, data_to_send.write_data_length);
}

void rtos_HW_I2C_Master::burst_byte_read(uint8_t slave_address, struct_RX_DataQueueI2C data_to_receive, uint16_t *dest)
{
    tx_dma->xfer_i2c2dma(this->i2c, slave_address, data_to_receive.mem_address, this->i2c_master_exclusive_irq_handler, dest, data_to_receive.read_data_length);
}

void rtos_HW_I2C_Master::i2c_dma_isr()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    irq_num_t irq_number = (this->i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    irq_set_enabled(irq_number, false); // disable IRQs to avoid re-entrance
    if (this->i2c->hw->intr_stat & I2C_IC_INTR_STAT_R_STOP_DET_BITS)
    {
        this->i2c->hw->clr_stop_det; // clear the STOP_DET interrupt
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
