/**
 * @file test_i2c_bus_scan.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <string.h>

#include "hw/i2c/hw_i2c.h"
#include "utilities/probe/probe.h"

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
    .sda_pin = 10,
    .scl_pin = 11,
    .baud_rate = I2C_STANDARD_MODE,
    .slave_address = 0x15,
    .slave_memory_size = 256,
    .handler = i2c_slave_handler};


HW_I2C_Master master = HW_I2C_Master(master_config);
HW_I2C_Slave slave = HW_I2C_Slave(slave_config);                          


/**
 * @brief the actual I2C IRQ handler given to the RP2040 hardware I2C instance
 * 
 * @param i2c Either i2c0 or i2c1
 * @param event the event that gives the type of received data/command
 */
static void i2c_slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event)
{
    pr_D4.hi();
    slave.slave_isr(event);
    pr_D4.lo();
}

void show_i2c_bus_map(std::set<uint8_t> device_address_set)
{
    printf("Connected slave's map \n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    for (size_t i = 0; i < 0x80; i++)
    {
        if (i % 16 == 0)
            printf("%02x ", i); // start of lines labels
        printf(device_address_set.contains(i) ? "@" : ((i < 0x08 || i > 0x78) ? " " : "."));
        printf(i % 16 == 15 ? "\n" : "  "); // columns interspace and end of lines
    }
    printf("done\n");
}


int main()
{
    stdio_init_all();
    slave.context.mem[slave_config.slave_memory_size-1]=slave_config.slave_address;//just for the fun:the slave answer its address located in its last memory
    printf("test I2C bus scan : ");
    show_i2c_bus_map(master.bus_scan());


}