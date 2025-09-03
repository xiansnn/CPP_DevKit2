#include <stdio.h>
#include <string.h>

#include "hw/dma/hw_dma.h"

#include "hw/i2c/rtos_hw_i2c.h"

#include "utilities/probe/probe.h"

Probe pr_D0 = Probe(0);
Probe pr_D1 = Probe(1);
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

uint channel_rx;

void i2c_tx_fifo_handler();

struct_ConfigMasterI2C master_config{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE,
    .i2c_master_handler = i2c_tx_fifo_handler
};

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

struct_ConfigSlaveI2C slave_config{
    .i2c = i2c1,
    .sda_pin = 14,
    .scl_pin = 15,
    .baud_rate = I2C_FAST_MODE,
    .slave_address = 0x15,
    .slave_memory_size = 256,
    .handler = i2c_slave_handler};

rtos_HW_I2C_Master master = rtos_HW_I2C_Master(master_config);
HW_I2C_Slave slave = HW_I2C_Slave(slave_config);

#define MAX_DATA_SIZE 32

int main()
{
    stdio_init_all();
    printf("Test I2C exchange \n");

    for (uint8_t mem_address = 0;; mem_address = (mem_address + MAX_DATA_SIZE) % slave_config.slave_memory_size)
    {
        //==================================================================================================================
        // write to mem_address
        //==================================================================================================================

        uint8_t write_data[MAX_DATA_SIZE]; // buffer for data to write, given to DMA
        char write_msg[MAX_DATA_SIZE]{0};
        snprintf(write_msg, MAX_DATA_SIZE, "Hello, slave@0x%02X mem[0x%02X]", slave_config.slave_address, mem_address);
        uint8_t write_msg_len = strlen(write_msg);

        for (size_t i = 0; i < write_msg_len; i++)
        {
            write_data[i] = (uint16_t)write_msg[i];
        }

        printf("Write %d at 0x%02X: '%s'\t", write_msg_len, mem_address, write_msg); //

        pr_D4.hi();
        master.burst_byte_write(slave_config.slave_address, mem_address, write_data, write_msg_len);
        pr_D4.lo();

        sleep_us(500);

        //==================================================================================================================
        // read from mem_address
        //==================================================================================================================

        // pr_D6.hi();
        // channel_rx = dma_claim_unused_channel(true);
        // uint8_t mem_add[1] = {mem_address};
        // dma_channel_config c_rx = dma_channel_get_default_config(channel_rx);
        // channel_config_set_transfer_data_size(&c_rx, DMA_SIZE_8);
        // channel_config_set_dreq(&c_rx, i2c_get_dreq(slave_config.i2c, false));
        // channel_config_set_read_increment(&c_rx, false);
        // channel_config_set_write_increment(&c_rx, true);
        // dma_channel_configure(channel_rx, &c_rx,
        //                       read_data,
        //                       &i2c_get_hw(slave_config.i2c)->data_cmd,
        //                       msg_len,                                                          // element count (each element is of size transfer_data_size)
        //                       false);                                                           // don't start yet
        // i2c_read_blocking(slave_config.i2c, slave_config.slave_address, mem_add, 1, true); // send command byte
        // dma_channel_start(channel_rx);
        // dma_channel_wait_for_finish_blocking(channel_rx);
        // pr_D6.lo();

        // ##############################
        //
        pr_D5.hi();
        uint8_t read_data[MAX_DATA_SIZE];
        char read_msg[MAX_DATA_SIZE]{0};
        uint32_t cmd[1];

        // write command, pass mem_address
        cmd[0] = mem_address | I2C_IC_DATA_CMD_RESTART_BITS | I2C_IC_DATA_CMD_STOP_BITS;
        master_config.i2c->hw->enable = 0;
        master_config.i2c->hw->tar = slave_config.slave_address;
        master_config.i2c->hw->enable = 1;

        master_config.i2c->hw->data_cmd = cmd[0];
        do
        {
            tight_loop_contents();
        } while (!(master_config.i2c->hw->raw_intr_stat & I2C_IC_RAW_INTR_STAT_TX_EMPTY_BITS));

        //
        // read data
        //
        for (int i = 0; i < write_msg_len; i++)
        {
            while (!i2c_get_write_available(master_config.i2c))
                tight_loop_contents();
            master_config.i2c->hw->data_cmd = I2C_IC_DATA_CMD_CMD_BITS |
                                              (i == write_msg_len - 1 ? I2C_IC_DATA_CMD_STOP_BITS : 0);

            while (!i2c_get_read_available(master_config.i2c))
                tight_loop_contents();
            read_data[i] = (uint8_t)master_config.i2c->hw->data_cmd;
        }
        //------------------------------
        memcpy(read_msg, read_data, write_msg_len);
        uint8_t read_msg_len = strlen(read_msg);
        pr_D5.lo();
        printf("Read %d char at 0x%02X: '%s'\n", read_msg_len, mem_address, read_msg);
        sleep_ms(100);
    }

    return 0;
}

void i2c_tx_fifo_handler()
{
    pr_D0.hi();
    master.i2c_tx_fifo_dma_isr();
    pr_D0.lo();
}
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    pr_D1.hi();
    slave.slave_isr(event);
    pr_D1.lo();
}
