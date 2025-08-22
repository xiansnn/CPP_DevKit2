/**
 * @file hw_spi.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-03-29
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "hardware/spi.h"

#include "pico/stdlib.h"

/**
 * @brief The configuration data of SPI interface used as Master
 */
struct struct_ConfigMasterSPI
{
    /// @brief the SPI interface used in the rp2040: spi0 or spi1
    spi_inst_t *spi = spi1;
    /// @brief the associated gpio pin for clock
    uint sck_pin = 10;
    /// @brief the associated gpio pin for TX (MOSI)
    uint tx_pin = 11;
    /// @brief the associated gpio pin for RX (MISO)
    uint rx_pin = 12;
    /// @brief the associated gpio pin for Chip Select
    uint cs_pin = 13;
    ///@brief the SPI baudrate(default to 1MHz)
    uint baud_rate_Hz = 1000 * 1000;
    /// @brief size of SPI trasfert. Should be between 4 and 16. Default to 8.
    uint transfer_size = 8;
    /// @brief clock phase (default to standard 0)
    spi_cpha_t clk_phase = SPI_CPHA_0;
    /// @brief signal polarity (default to standard 0)
    spi_cpol_t spi_polarity = SPI_CPOL_0;
    /// @brief bit order (default to MSB first)
    spi_order_t bit_order = SPI_MSB_FIRST;
};

/**
 * @brief this is a C++ wrapper for the original pico SDK spi master API
 * \ingroup hw
 */
class HW_SPI_Master
{
private:
    /// @brief the spi hardware instance of the Pico that handles the spi master
    spi_inst_t *spi;

public:
     /**
      * @brief Construct a new hw spi master object
      * 
      * @param master_config 
      */
    HW_SPI_Master(struct_ConfigMasterSPI master_config);
    ~HW_SPI_Master();

    /**
     * @brief send a serie of 8 bit data on the SPI MOSI port
     *
     * @param src the source of the data
     * @param len the lenght of the serie
     * @return int the number of byte sent
     */
    int burst_write_8(uint8_t *src, size_t len);
    /**
     * @brief send a serie of 16 bit data on the SPI MOSI port
     *
     * @param src the source of the data
     * @param len the lenght of the serie
     * @return int the number of byte sent
     */
    int burst_write_16(uint16_t *src, size_t len);

    /**
     * @brief read / write a serie of 8 bit data on the SPI MISO/MOSI ports
     *
     * @param src the source of the data
     * @param dest the destination of the data
     * @param len the length of the serie
     * @return int
     */
    int burst_write_read_8(uint8_t *src, uint8_t *dest, size_t len);

    /**
     * @brief write a single byte on the SPI MOSI port
     *
     * @param byte_value
     * @return int
     */
    int single_write_8(uint8_t byte_value);
    /**
     * @brief write a single 16bit word on the SPI MOSI port
     *
     * @param word_value
     * @return int
     */
    int single_write_16(uint16_t word_value);

    /**
     * @brief read a single byte on the SPI MISO port
     *
     * @param dest the destination that receives the data
     * @return int
     */
    int single_read_8(uint8_t *dest);

    /**
     * @brief read a serie of byte on the SPI MISO port
     *
     * @param repeated_tx_data the byte sent on MOSI port during the read transfer
     * @param dest the destination that receives the data
     * @param len the length of the serie
     * @return int
     */
    int burst_read_8(uint8_t repeated_tx_data, uint8_t *dest, size_t len);

    /**
     * @brief a test that checks if the slave_address device is connected
     *
     * @param slave_address
     * @return true
     * @return false
     */
    bool device_is_connected(uint8_t slave_address);
};