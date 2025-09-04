
#include "rtos_hw_i2c.h"

rtos_HW_I2C_Master::rtos_HW_I2C_Master(struct_ConfigMasterI2C cfg)
    : HW_I2C_Master(cfg)
{
    this->tx_dma = new HW_DMA();
    i2c_tx_data_queue = xQueueCreate(8, sizeof(struct_TX_DataQueueI2C));
}

rtos_HW_I2C_Master::~rtos_HW_I2C_Master()
{
}

void rtos_HW_I2C_Master::burst_byte_write(uint8_t slave_address, struct_TX_DataQueueI2C data_to_send)//    uint8_t slave_mem_addr, uint8_t *src, size_t len)
{
    tx_dma->write_dma2i2c(this->i2c, slave_address, data_to_send.mem_address, this->i2c_master_handler,data_to_send.write_data, data_to_send.write_data_length);
}

void rtos_HW_I2C_Master::burst_byte_read(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *src, size_t len)
{
}

void rtos_HW_I2C_Master::i2c_tx_fifo_dma_isr()
{
    irq_num_t irq_number = (this->i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    irq_set_enabled(irq_number, false);                               // disable IRQs to avoid re-entrance
    if (this->i2c->hw->intr_stat && I2C_IC_INTR_STAT_R_TX_EMPTY_BITS)
        xTaskNotifyGive(this->task_to_notify_when_fifo_empty);
}
