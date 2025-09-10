
#include "rtos_hw_i2c.h"
#include "utilities/probe/probe.h"
// Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

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

void rtos_HW_I2C_Master::burst_byte_write(uint8_t slave_address, struct_TX_DataQueueI2C data_to_send) //    uint8_t slave_mem_addr, uint8_t *src, size_t len)
{
    tx_dma->xfer_dma2i2c(this->i2c, slave_address, data_to_send.mem_address, this->i2c_master_exclusive_irq_handler, data_to_send.write_data, data_to_send.write_data_length);
}

void rtos_HW_I2C_Master::burst_byte_read(uint8_t slave_address, struct_RX_DataQueueI2C data_to_receive, uint16_t *dest)
{
    tx_dma->xfer_i2c2dma(this->i2c, slave_address, data_to_receive.mem_address, this->i2c_master_exclusive_irq_handler, dest, data_to_receive.read_data_length);
}

void rtos_HW_I2C_Master::i2c_tx_fifo_dma_isr()
{
    irq_num_t irq_number = (this->i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    irq_set_enabled(irq_number, false); // disable IRQs to avoid re-entrance
    if (this->i2c->hw->intr_stat && I2C_IC_INTR_STAT_R_TX_EMPTY_BITS)
        xTaskNotifyGive(this->TX_task_to_notify_when_fifo_empty);
}

void rtos_HW_I2C_Master::i2c_rx_fifo_dma_isr()
{
    irq_num_t irq_number = (this->i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    irq_set_enabled(irq_number, false); // disable IRQs to avoid re-entrance
    if (this->i2c->hw->intr_stat && I2C_IC_INTR_STAT_R_TX_EMPTY_BITS)
        xTaskNotifyGive(this->RX_task_to_notify_when_fifo_empty);
}
