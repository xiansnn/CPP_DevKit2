#include <stdio.h>
#include <string.h>
#include "hw_i2c.h"
#include "probe.h"

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

struct_ConfigMasterI2C master_config{
    .i2c = i2c0,
    .sda_pin = 8,
    .scl_pin = 9,
    .baud_rate = I2C_FAST_MODE
    };

static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event);

struct_ConfigSlaveI2C slave_config{
    .i2c = i2c1,
    .sda_pin = 6,
    .scl_pin = 7,
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
            uint8_t write_data[MAX_DATA_SIZE];
            char write_msg[MAX_DATA_SIZE];
            snprintf(write_msg, sizeof(write_msg), "Hello, slave@0x%02X mem[0x%02X]", slave_config.slave_address, mem_address);
            uint8_t msg_len = strlen(write_msg);
            memcpy(write_data, write_msg, msg_len); // to convert  char[] to uint8_t[]
            // write data at mem_address
            printf("Write at 0x%02X: '%s'\n", mem_address, write_msg);
            pr_D5.hi();
            master.burst_byte_write(slave_config.slave_address, mem_address, write_data, msg_len);
            pr_D5.lo();

            // read from mem_address
            uint8_t read_data[MAX_DATA_SIZE];
            char read_msg[MAX_DATA_SIZE];
            master.burst_byte_read(slave_config.slave_address, mem_address, read_data, MAX_DATA_SIZE);
            memcpy(read_msg, read_data, MAX_DATA_SIZE);
            msg_len = strlen(read_msg);
            printf("Read %d char at 0x%02X: '%s'\n\n", msg_len, mem_address, read_msg);
            sleep_ms(1000);
        }
    }
    return 0;
}
