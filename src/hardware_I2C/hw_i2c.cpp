/**
 * @file hw_i2c.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief this file provides an abstract view of the I2C hardware instances.
 * It must be considered as a set of helper to the official pico-sdk.
 * @version 0.1
 * @date 2023-05-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "hw_i2c.h"
#include <cstring>
#include <stdio.h>
#include <string>

HW_I2C_Master::HW_I2C_Master(struct_ConfigMasterI2C cfg)
{
    this->i2c = cfg.i2c;
    this->time_out_us_per_byte = 8 * 1500000 / cfg.baud_rate; // with 50% margin

    // As suggested by RP2040 data sheet
    gpio_init(cfg.sda_pin);
    gpio_set_function(cfg.sda_pin, GPIO_FUNC_I2C);
    gpio_pull_up(cfg.sda_pin);
    gpio_set_slew_rate(cfg.sda_pin, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(cfg.sda_pin, true);

    gpio_init(cfg.scl_pin);
    gpio_set_function(cfg.scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(cfg.scl_pin);
    gpio_set_slew_rate(cfg.scl_pin, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(cfg.scl_pin, true);

    i2c_init(this->i2c, cfg.baud_rate);
}

struct_I2CXferResult HW_I2C_Master::burst_byte_write(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *src, size_t len)
{
    size_t nb;
    struct_I2CXferResult result;
    uint8_t write_buf[len + 1] = {slave_mem_addr};
    memcpy(write_buf + 1, src, len);
    // nb = i2c_write_blocking(this->i2c, slave_address, write_buf, len + 1, false);
    uint timeout = this->time_out_us_per_byte * (len + 3);
    nb = i2c_write_timeout_us(this->i2c, slave_address, write_buf, len + 1, false, timeout);
    result.xfer_size = nb;
    if (nb < 0 || nb != len + 1)
    {
        result.context = "burst_byte_write [write cmd][data*]";
        result.error = true;
        return result;
    }
    result.error = false;
    return result;
}

struct_I2CXferResult HW_I2C_Master::single_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t mem_value)
{
    int nb;
    struct_I2CXferResult result;
    uint8_t write_buf[] = {mem_addr, mem_value};
    // nb = i2c_write_blocking(this->i2c, slave_address, write_buf, 2, false);
    uint timeout = this->time_out_us_per_byte * 3;
    nb = i2c_write_timeout_us(this->i2c, slave_address, write_buf, 2, false, timeout);
    result.xfer_size = nb;
    if (nb < 0 || nb != 2)
    {
        result.context = "single_byte_write [write cmd][byte]";
        result.error = true;
        return result;
    }
    result.error = false;
    return result;
}

struct_I2CXferResult HW_I2C_Master::single_byte_read(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *dest)
{
    int nb;
    struct_I2CXferResult result;
    uint8_t cmd_buf[]{slave_mem_addr};

    // i2c_write_blocking(this->i2c, slave_address, cmd_buf, 1, true);
    uint timeout = this->time_out_us_per_byte * 2;
    nb = i2c_write_timeout_us(this->i2c, slave_address, cmd_buf, 1, true, timeout);
    result.xfer_size = nb;
    if (nb < 0 || nb != 1)
    {
        result.context = "single_byte_read [write cmd]";
        result.error = true;
        return result;
    }
    // nb = i2c_read_blocking(this->i2c, slave_address, dest, 1, false);
    timeout = this->time_out_us_per_byte * 3;
    nb = i2c_read_timeout_us(this->i2c, slave_address, dest, 1, false, timeout);
    result.xfer_size = nb;
    if (nb < 0 || nb != 1)
    {
        result.context = "single_byte_read [read byte]";
        result.error = true;
        return result;
    }
    result.error = false;
    return result;
}

struct_I2CXferResult HW_I2C_Master::burst_byte_read(uint8_t slave_address, uint8_t slave_mem_addr, uint8_t *dest, size_t len)
{
    size_t nb;
    struct_I2CXferResult result;
    uint8_t cmd_buf[]{slave_mem_addr};
    // i2c_write_blocking(this->i2c, slave_address, cmd_buf, 1, true);
    uint timeout = this->time_out_us_per_byte * 2;
    nb = i2c_write_timeout_us(this->i2c, slave_address, cmd_buf, 1, true, timeout);
    result.xfer_size = nb;
    if (nb < 0 || nb != 1)
    {
        result.context = "burst_byte_read [write cmd]";
        result.error = true;
        return result;
    }

    // int nb = i2c_read_blocking(this->i2c, slave_address, dest, len, false);
    timeout = this->time_out_us_per_byte * (len + 2);
    nb = i2c_read_timeout_us(this->i2c, slave_address, dest, len, false, timeout);
    result.xfer_size = nb;
    if (nb < 0 || nb != len)
    {
        result.context = "burst_byte_read [read data*]";
        result.error = true;
        return result;
    }
    result.error = false;
    return result;
}

std::set<uint8_t> HW_I2C_Master::bus_scan()
{
    std::set<uint8_t> device_address_set;
    for (uint8_t addr = 0x08; addr < 0x78; addr++)
        if (this->device_is_connected(addr))
            device_address_set.insert(addr);
    return device_address_set;
}
bool HW_I2C_Master::device_is_connected(uint8_t slave_address)
{
    int nb;
    uint8_t rxdata;
    nb = i2c_read_blocking(this->i2c, slave_address, &rxdata, 1, false);
    return (nb < 0 ? false : true);
}

void HW_I2C_Master::show_bus_map()
{
    printf("Connected slave's map \n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    std::set<uint8_t> device_address_set = this->bus_scan();
    for (size_t i = 0; i < 0x80; i++)
    {
        if (i % 16 == 0)
            printf("%02x ", i); // start of lines labels
        printf(device_address_set.contains(i) ? "@" : ((i < 0x08 || i > 0x78) ? " " : "."));
        printf(i % 16 == 15 ? "\n" : "  "); // columns interspace and end of lines
    }
    printf("done\n");
}

void HW_I2C_Slave::slave_isr(i2c_slave_event_t event)
{
    switch (event)
    {
    case I2C_SLAVE_RECEIVE: // master has sent a Write command ...
        if (!this->context.mem_address_written)
        {
            // ... for the  first time. This gives the memory start address
            this->context.mem_address = i2c_read_byte_raw(this->i2c);
            this->context.mem_address_written = true; // to remind a write was received
        }
        else
        {
            // ... not the firrst time. This means the slave must copy the received data in its memory
            this->context.mem[context.mem_address] = i2c_read_byte_raw(this->i2c);
            this->context.mem_address++; // prepare for the next received data
        }
        break;
    case I2C_SLAVE_REQUEST: // master is requesting data
        // the starting address was given by the first Write command
        i2c_write_byte_raw(this->i2c, this->context.mem[context.mem_address]);
        this->context.mem_address++;
        break;
    case I2C_SLAVE_FINISH: // master has signalled Stop / Restart
        this->context.mem_address_written = false;
        break;
    default:
        break;
    }
}

HW_I2C_Slave::HW_I2C_Slave(struct_ConfigSlaveI2C cfg)
{
    this->i2c = cfg.i2c;

    // As suggested by RP2040 data sheet
    gpio_init(cfg.sda_pin);
    gpio_set_function(cfg.sda_pin, GPIO_FUNC_I2C);
    gpio_pull_up(cfg.sda_pin);
    gpio_set_slew_rate(cfg.sda_pin, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(cfg.sda_pin, true);

    gpio_init(cfg.scl_pin);
    gpio_set_function(cfg.scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(cfg.scl_pin);
    gpio_set_slew_rate(cfg.scl_pin, GPIO_SLEW_RATE_SLOW);
    gpio_set_input_hysteresis_enabled(cfg.scl_pin, true);

    i2c_init(this->i2c, cfg.baud_rate);

    i2c_slave_init(this->i2c, cfg.slave_address, cfg.handler);
}
