#include <stdio.h>
#include <string.h>

#include "hw/i2c/hw_i2c.h"

#include "hw/dma/hw_dma.h"

#include "utilities/probe/probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);

uint channel_tx;
uint channel_rx;

struct_ConfigMasterI2C master_config{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE};

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

struct_ConfigSlaveI2C slave_config{
    .i2c = i2c1,
    .sda_pin = 14,
    .scl_pin = 15,
    .baud_rate = I2C_FAST_MODE,
    .slave_address = 0x15,
    .slave_memory_size = 256,
    .handler = i2c_slave_handler};

HW_I2C_Master master = HW_I2C_Master(master_config);
HW_I2C_Slave slave = HW_I2C_Slave(slave_config);

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    pr_D4.hi();
    slave.slave_isr(event);
    pr_D4.lo();
}

#define MAX_DATA_SIZE 32

int main()
{
    stdio_init_all();
    printf("test I2C exchange \n");
    if (not master.device_is_connected(slave_config.slave_address))
    {
        printf("no device at 0x%02x\n", slave_config.slave_address);
    }
    else
    {
        for (uint8_t mem_address = 0;; mem_address = (mem_address + MAX_DATA_SIZE) % slave_config.slave_memory_size)
        {
            uint8_t write_data[MAX_DATA_SIZE]; // buffer for data to write, given to DMA
            char write_msg[MAX_DATA_SIZE];
            snprintf(write_msg, sizeof(write_msg), "Hello, slave@0x%02X mem[0x%02X]", slave_config.slave_address, mem_address);
            uint8_t msg_len = strlen(write_msg);
            memcpy(write_data, write_msg, msg_len); // to convert  char[] to uint8_t[]

            printf("Write at 0x%02X: '%s'\t", mem_address, write_msg); // FIXME first char missing on terminal
            //------------------------------
            // pr_D5.hi();
            // uint32_t full_buffer[MAX_DATA_SIZE + 1];
            // full_buffer[0] = mem_address; // first byte is reserved for command (start/stop)
            // for (size_t i = 0; i < msg_len; i++)
            // {
            //     full_buffer[i + 1] = write_msg[i];
            // }
            // full_buffer[1] |= I2C_IC_DATA_CMD_FIRST_DATA_BYTE_BITS;
            // full_buffer[msg_len] |= I2C_IC_DATA_CMD_STOP_BITS;

            // channel_tx = dma_claim_unused_channel(true);
            // dma_channel_cleanup(channel_tx);
            // dma_channel_config c_tx = dma_channel_get_default_config(channel_tx);
            // // channel_config_set_transfer_data_size(&c_tx, DMA_SIZE_32);
            // channel_config_set_transfer_data_size(&c_tx, DMA_SIZE_8);
            // channel_config_set_dreq(&c_tx, i2c_get_dreq(master_config.i2c, true));
            // channel_config_set_read_increment(&c_tx, true);
            // channel_config_set_write_increment(&c_tx, false);
            // uint32_t transfer_count = msg_len + 1;
            // // dma_channel_configure(channel_tx, &c_tx,
            // //                       &i2c_get_hw(master_config.i2c)->data_cmd,
            // //                       full_buffer,    // read address
            // //                       transfer_count, // element count (each element is of size transfer_data_size)
            // //                       false);         // don't start yet
            // dma_channel_configure(channel_tx, &c_tx,
            //                       &i2c_get_hw(master_config.i2c)->data_cmd,
            //                       write_data, // read address
            //                       msg_len,    // element count (each element is of size transfer_data_size)
            //                       false);     // don't start yet
            // i2c_write_blocking(master_config.i2c, slave_config.slave_address, &mem_address, 1, true); // send command byte
            // dma_channel_start(channel_tx);
            // dma_channel_wait_for_finish_blocking(channel_tx);
            // dma_channel_cleanup(channel_tx);
            // dma_channel_unclaim(channel_tx);
            // pr_D5.lo();
            //------------------------------
            pr_D5.hi();
            master.burst_byte_write(slave_config.slave_address, mem_address, write_data, msg_len);
            pr_D5.lo();
            //------------------------------

            // read from mem_address
            sleep_ms(1);

            uint8_t read_data[MAX_DATA_SIZE];
            char read_msg[MAX_DATA_SIZE];
            //------------------------------
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
            //------------------------------
            pr_D6.hi();
            master.burst_byte_read(slave_config.slave_address, mem_address, read_data, msg_len);
            pr_D6.lo();
            //------------------------------
            memcpy(read_msg, read_data, msg_len);
            msg_len = strlen(read_msg);
            printf("Read %d char at 0x%02X: '%s'\n", msg_len, mem_address, read_msg);
            sleep_ms(1000);
        }
    }
    return 0;
}
