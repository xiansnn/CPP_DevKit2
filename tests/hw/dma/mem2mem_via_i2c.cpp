#include <stdio.h>
#include <string.h>

#include "hw/i2c/hw_i2c.h"

#include "hw/dma/hw_dma.h"

#include "utilities/probe/probe.h"

void i2c_send_memory_address_and_start_bit(i2c_inst_t *i2c, uint8_t slave_address, uint8_t mem_address);
void i2c_send_stop(i2c_inst_t *i2c, uint8_t slave_address);

Probe pr_D0 = Probe(0);
Probe pr_D1 = Probe(1);
Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);
Probe pr_D6 = Probe(6);
Probe pr_D7 = Probe(7);

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
    // pr_D1.hi();
    slave.slave_isr(event);
    // pr_D1.lo();
}

dma_channel_config c_tx;
uint16_t *tx_ptr;
size_t tx_remaining;
size_t chunk;
bool fifo_empty;

#define MAX_DATA_SIZE 32
#define BURST_SIZE 16
// #define WATERMARK_LEVEL 16 - BURST_SIZE
// #define BYTE_TX_TIME (10 * 1e6 / I2C_FAST_MODE) // in µs, with 50% margin

void i2c_tx_fifo_handler();

int main()
{
    stdio_init_all();
    printf("Test I2C exchange \n");
    // if (not master.device_is_connected(slave_config.slave_address))
    // {
    //     printf("no device at 0x%02x\n", slave_config.slave_address);
    // }
    // else
    // {

    // set fifo WATERMARK_LEVEL
    // master_config.i2c->hw->tx_tl = WATERMARK_LEVEL;
    // master_config.i2c->hw->rx_tl = WATERMARK_LEVEL;
    // master_config.i2c->hw->con |= I2C_IC_CON_TX_EMPTY_CTRL_BITS;

    for (uint8_t mem_address = 0;; mem_address = (mem_address + MAX_DATA_SIZE) % slave_config.slave_memory_size)
    {
        //==================================================================================================================
        // write to mem_address
        //==================================================================================================================

        uint16_t write_data[MAX_DATA_SIZE]; // buffer for data to write, given to DMA
        char write_msg[MAX_DATA_SIZE]{0};
        snprintf(write_msg, MAX_DATA_SIZE, "Hello, slave@0x%02X mem[0x%02X]", slave_config.slave_address, mem_address);
        uint8_t write_msg_len = strlen(write_msg);

        for (size_t i = 0; i < write_msg_len; i++)
        {
            write_data[i] = (uint16_t)write_msg[i];
        }

        printf("Write %d at 0x%02X: '%s'\t", write_msg_len, mem_address, write_msg); //

        pr_D4.hi();
        channel_tx = dma_claim_unused_channel(true);
        dma_channel_cleanup(channel_tx);
        dma_channel_config c_tx = dma_channel_get_default_config(channel_tx);
        channel_config_set_transfer_data_size(&c_tx, DMA_SIZE_16);
        channel_config_set_dreq(&c_tx, i2c_get_dreq(master_config.i2c, true));
        channel_config_set_read_increment(&c_tx, true);
        channel_config_set_write_increment(&c_tx, false);

        master_config.i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_TX_EMPTY_BITS;
        irq_set_exclusive_handler(I2C0_IRQ, i2c_tx_fifo_handler);
        irq_set_enabled(I2C0_IRQ, true);

        pr_D7.hi();
        fifo_empty = false;
        i2c_send_memory_address_and_start_bit(master_config.i2c, slave_config.slave_address, mem_address);

        pr_D6.hi();
        irq_set_enabled(I2C0_IRQ, true);
        while (!fifo_empty)
            tight_loop_contents();
        pr_D6.lo();
        pr_D7.lo();

        tx_ptr = write_data;
        tx_remaining = write_msg_len;
        while (tx_remaining > 0)
        {

            chunk = (tx_remaining > BURST_SIZE) ? BURST_SIZE : tx_remaining;

            fifo_empty = false;
            pr_D1.hi();
            dma_channel_configure(channel_tx, &c_tx,
                                  &i2c_get_hw(master_config.i2c)->data_cmd,
                                  tx_ptr, // read address
                                  chunk,  // element count (each element is of size transfer_data_size)
                                  true);  // don't start yet
            tx_ptr += chunk;
            tx_remaining -= chunk;
            pr_D1.lo();

            pr_D6.hi();
            irq_set_enabled(I2C0_IRQ, true);
            while (!fifo_empty)
                tight_loop_contents();
            pr_D6.lo();
        }

        dma_channel_cleanup(channel_tx);
        dma_channel_unclaim(channel_tx);

        pr_D7.hi();
        fifo_empty = false;
        i2c_send_stop(master_config.i2c, slave_config.slave_address);

        pr_D6.hi();
        irq_set_enabled(I2C0_IRQ, true);
        while (!fifo_empty)
            tight_loop_contents();
        pr_D6.lo();

        pr_D7.lo();

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
    // }
    return 0;
}

void i2c_send_memory_address_and_start_bit(i2c_inst_t *i2c, uint8_t slave_address, uint8_t mem_address)
{
    i2c->hw->enable = 0;
    i2c->hw->tar = slave_address;
    i2c->hw->enable = 1;
    i2c->hw->data_cmd = mem_address | I2C_IC_DATA_CMD_RESTART_BITS;
}

// Envoie un STOP condition sans transmettre de données
// Utilise une lecture d'un octet jetable pour forcer le STOP
void i2c_send_stop(i2c_inst_t *i2c, uint8_t slave_address)
{
    uint8_t dummy;
    int result = i2c_read_blocking(i2c, slave_address, &dummy, 1, false); // false = STOP
    if (result < 0)
    {
        panic("Erreur lors de l'envoi du STOP condition\n");
    }
}
void i2c_tx_fifo_handler()
{
    irq_set_enabled(I2C0_IRQ, false); // disable IRQs to avoid re-entrance

    if (master_config.i2c->hw->intr_stat && I2C_IC_INTR_STAT_R_TX_EMPTY_BITS)
    {
        pr_D0.hi();
        fifo_empty = true;
        pr_D0.lo();
    }
}
