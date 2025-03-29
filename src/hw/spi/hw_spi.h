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
// #include "spi/hw_spi.h"
// #include <spi.h>

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
    HW_SPI_Master(struct_ConfigMasterSPI master_config);
    ~HW_SPI_Master();

    int burst_byte_write(uint8_t *src, size_t len);

    int burst_byte_write_read(uint8_t *src, uint8_t *dest, size_t len);

    int single_byte_write(uint8_t byte_value);

    int single_byte_read(uint8_t *dest);

    int burst_byte_read(uint8_t repeated_tx_data,  uint8_t *dest, size_t len);

    bool device_is_connected(uint8_t slave_address);
};