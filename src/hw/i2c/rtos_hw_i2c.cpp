
#include "rtos_hw_i2c.h"
#include "utilities/probe/probe.h"
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

rtos_HW_I2C_Master::rtos_HW_I2C_Master(struct_ConfigMasterI2C cfg)
    : HW_I2C_Master(cfg)
{
    this->tx_dma = new HW_DMA();
    i2c_tx_data_queue = xQueueCreate(8, sizeof(struct_TX_DataQueueI2C));
}

rtos_HW_I2C_Master::~rtos_HW_I2C_Master()
{
}

void rtos_HW_I2C_Master::burst_byte_write(uint8_t slave_address, struct_TX_DataQueueI2C data_to_send) //    uint8_t slave_mem_addr, uint8_t *src, size_t len)
{
    tx_dma->xfer_dma2i2c(this->i2c, slave_address, data_to_send.mem_address, this->i2c_master_handler, data_to_send.write_data, data_to_send.write_data_length);
}

void rtos_HW_I2C_Master::burst_byte_read(uint8_t slave_address, struct_RX_DataQueueI2C data_to_receive, uint8_t *dest)
{
    uint32_t cmd[1];

    // write command, pass mem_address
    this->i2c->hw->enable = 0;
    this->i2c->hw->tar = slave_address;
    this->i2c->hw->enable = 1;
    cmd[0] = data_to_receive.mem_address | I2C_IC_DATA_CMD_RESTART_BITS | I2C_IC_DATA_CMD_STOP_BITS;
    this->i2c->hw->data_cmd = cmd[0];

    pr_D7.hi();
    tx_dma->xfer_i2c2dma(this->i2c, slave_address, data_to_receive.mem_address, this->i2c_master_handler, dest, data_to_receive.read_data_length);
    pr_D7.lo();

    size_t chunk;
    size_t rx_remaining = data_to_receive.read_data_length;
    size_t received_data_index = 0;
    size_t tx_cmd_index = 0;
    // read data
    while (rx_remaining > 0)
    { // wait until the command is sent
        while (!(this->i2c->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS))
            tight_loop_contents();

        chunk = (rx_remaining > I2C_BURST_SIZE) ? I2C_BURST_SIZE : rx_remaining;

        for (size_t i = 0; i < chunk; i++)
        {
            pr_D6.hi();
            uint32_t cmd = I2C_IC_DATA_CMD_CMD_BITS | (tx_cmd_index == data_to_receive.read_data_length - 1 ? I2C_IC_DATA_CMD_STOP_BITS : 0);
            i2c->hw->data_cmd = cmd;
            tx_cmd_index++;
            pr_D6.lo();
        }
        rx_remaining -= chunk;

        // read chunk of data -> prepare for DMA

        // for (size_t i = 0; i < chunk; i++)
        // {
        //     pr_D7.hi();
        //     while (!i2c->hw->rxflr)
        //         tight_loop_contents();
        //     read_data[received_data_index] = (uint8_t)i2c->hw->data_cmd;
        //     received_data_index++;
        //     pr_D7.lo();
        // }
    }
    // while (this->i2c->hw->rxflr != 0)
    //     tight_loop_contents();
}

void rtos_HW_I2C_Master::i2c_tx_fifo_dma_isr()
{
    irq_num_t irq_number = (this->i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    irq_set_enabled(irq_number, false); // disable IRQs to avoid re-entrance
    if (this->i2c->hw->intr_stat && I2C_IC_INTR_STAT_R_TX_EMPTY_BITS)
        xTaskNotifyGive(this->task_to_notify_when_fifo_empty);
}
