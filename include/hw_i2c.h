/**
 * @file hw_i2c.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#if !defined(HW_I2C_H)
#define HW_I2C_H

#include "hardware/i2c.h"
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"
#include <set>
#include <string>

/// @brief I2C standard mode speed 100kb/s
#define I2C_STANDARD_MODE 100 * 1000
/// @brief I2C fast mode speed 400kb/s
#define I2C_FAST_MODE 400 * 1000
/// @brief I2C fast mode plus speed 1Mb/s
#define I2C_FAST_MODE_PLUS 1000 * 1000
/// @brief I2C max buffer memory size, default to 256 words
#define I2C_SLAVE_DEFAULT_MAX_MEMORY_SIZE 256

/**
 * @brief this data structure collects result when an I2C transfer is done.
 * It gives a more verbose error description : a context and the number of transfered byte.
 */
struct struct_I2CXferResult
{
    /**
     * @brief flag that indicates if an error occurred.
     *
     */
    bool error = false;
    /**
     * @brief a clear indication of where the error occurred:
     * - "burst_byte_write [write cmd][data*]"
     * - "single_byte_write [write cmd][byte]"
     * - "single_byte_read [write cmd]"
     * - "single_byte_read [read byte]"
     * - "burst_byte_read [write cmd]"
     * - "burst_byte_read [read data*]"
     *
     */
    std::string context = "";
    /**
     * @brief the number of byte transfered.
     *
     */
    int xfer_size = 0;
};

/**
 * @brief The configuration data of I2C interface used as Master
 */
struct struct_ConfigMasterI2C
{
    /**
     * @brief the I2C interface used in the rp2040: i2c0 or i2c1
     */
    i2c_inst_t *i2c = i2c0;
    /**
     * @brief the associated gpio pin for data
     */
    uint sda_pin = 04;
    /**
     * @brief the associated gpio pin for clock
     */
    uint scl_pin = 05;
    /**
     * @brief the I2C baudrate:
     * - I2C_STANDARD_MODE = 100kb/s
     * - I2C_FAST_MODE = 400kb/s
     * - I2C_FAST_MODE_PLUS = 1Mb/s
     */
    uint baud_rate = I2C_STANDARD_MODE;
};

/**
 * @brief The configuration data of I2C interface used as Slave
 */
struct struct_ConfigSlaveI2C
{
    /**
     * @brief the I2C interface used in the rp2040: i2c0 or i2c1
     */
    i2c_inst_t *i2c = i2c1;
    /**
     * @brief the associated gpio pin for data
     */
    uint sda_pin = 06;
    /**
     * @brief the associated gpio pin for clock
     */
    uint scl_pin = 07;
    /**
     * @brief the I2C baudrate:
     * - I2C_STANDARD_MODE = 100kb/s
     * - I2C_FAST_MODE = 400kb/s
     * - I2C_FAST_MODE_PLUS = 1Mb/s
     */
    uint baud_rate = I2C_STANDARD_MODE;
    /**
     * @brief the i2c slave address at which the interface responds
     */
    uint8_t slave_address = 0x55;
    /**
     * @brief the size of the memory buffer reserved by the slave interface (default to 256)
     */
    size_t slave_memory_size = I2C_SLAVE_DEFAULT_MAX_MEMORY_SIZE;
    /**
     * @brief a function pointer to the IRQ handler, required by pico SDK, to the program that manage the reception of i2c message by the slave interface
     *
     * NOTICE: This handler is the one given to NVIC IRQ map.
     * It seems that it must be a static function defined in the main code.
     */
    i2c_slave_handler_t handler;
};

/**
 * @brief this is a C++ wrapper for the original pico SDK i2c master API
 */
class HW_I2C_Master
{
private:
    /// @brief the i2c hardware instance of the Pico that handles the i2c master
    i2c_inst_t *i2c;
    /// @brief the delay before time-out allowed for reading i2c response. Computed according to actual baud rate.
    uint time_out_us_per_byte;

public:
    /**
     * @brief Construct a new hw i2c master object
     *
     * @param master_config
     */
    HW_I2C_Master(struct_ConfigMasterI2C master_config);
    /**
     * @brief a convenient C++ member wrapper to write a block of data starting at a slave memory address.
     * The operation is bounded by a timeout
     * @param slave_address the slave address
     * @param mem_addr the slave memory
     * @param src the address of the block of data
     * @param len the size of the block of data
     * @return struct_I2CXferResult
     */
    struct_I2CXferResult burst_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t *src, size_t len);
    /**
     * @brief a convenient C++ member wrapper to write a single byte at a slave memory address.
     * The operation is bounded by a timeout.
     *
     * @param slave_address the slave address
     * @param mem_addr the slave memory
     * @param mem_value the byte to write
     * @return struct_I2CXferResult
     */
    struct_I2CXferResult single_byte_write(uint8_t slave_address, uint8_t mem_addr, uint8_t mem_value);
    /**
     * @brief a convenient C++ member wrapper to read a single byte at a slave memory address.
     * The operation is bounded by a timeout.
     * @param slave_address the slave address
     * @param mem_addr the address of slave memory to read from
     * @param dest Pointer to buffer to receive data
     * @return struct_I2CXferResult
     */
    struct_I2CXferResult single_byte_read(uint8_t slave_address, uint8_t mem_addr, uint8_t *dest);
    /**
     * @brief a convenient C++ member wrapper to read a block of data starting at a slave memory address.
     * The operation is bounded by a timeout.
     * @param slave_address the slave address
     * @param mem_addr the starting address of slave memory to read from
     * @param dest Pointer to buffer to receive data
     * @param len the size of the block of data
     * @return struct_I2CXferResult
     */
    struct_I2CXferResult burst_byte_read(uint8_t slave_address, uint8_t mem_addr, uint8_t *dest, size_t len);
    /**
     * @brief A utility that scan the I2C bus and return the set of answering devices
     *
     * @return std::set<uint8_t> the set of responding addresses
     */
    std::set<uint8_t> bus_scan();
    /**
     * @brief utility to know if a device is connected
     *
     * @param slave_address
     * @return true the device responds ACK to a write
     * @return false no response
     */
    bool device_is_connected(uint8_t slave_address);
};

/**
 * @brief this is the structure of the memory used by the slave i2c interface
 *
 */
struct struct_SlaveMemory
{
    /// @brief the size of the slave memory buffer
    uint8_t mem[I2C_SLAVE_DEFAULT_MAX_MEMORY_SIZE]{};
    /// @brief the addresse where the slave read/write. this chages for each exchange sequence, the slave points to its last memory address
    uint8_t mem_address = I2C_SLAVE_DEFAULT_MAX_MEMORY_SIZE - 1;
    /// @brief falg that determine if the slave read or write to its memory buffer
    bool mem_address_written = false;
};

/**
 * @brief this is a C++ wrapper for the original pico SDK i2c slave API
 *
 */
class HW_I2C_Slave
{
private:
    /// @brief the i2c hardware instance of the Pico that handles the i2c slave
    i2c_inst_t *i2c;

public:
    /**
     * @brief a data structure used to configure the slave device
     *
     */
    struct_SlaveMemory context;
    /**
     * @brief Construct a new hw i2c slave object
     *
     * @param slave_config
     */
    HW_I2C_Slave(struct_ConfigSlaveI2C slave_config);
    /**
     * @brief this is the actual Interrupt Service Routine executed by the slave after each received data
     *
     * @param event the type of data/command received
     */
    void slave_isr(i2c_slave_event_t event);
};

#endif // HW_I2C_H
