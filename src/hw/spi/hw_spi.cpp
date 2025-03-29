#include "hw_spi.h"

HW_SPI_Master::HW_SPI_Master(struct_ConfigMasterSPI master_config)
{
    this->spi = master_config.spi;
    spi_init(spi, master_config.baud_rate_Hz);
    gpio_set_function(master_config.sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(master_config.rx_pin, GPIO_FUNC_SPI);
    gpio_set_function(master_config.tx_pin, GPIO_FUNC_SPI);
    gpio_set_function(master_config.cs_pin, GPIO_FUNC_SPI);
    spi_set_format(spi, 8, master_config.spi_polarity, master_config.clk_phase, master_config.bit_order);
}

HW_SPI_Master::~HW_SPI_Master()
{
    spi_deinit(spi);
}

int HW_SPI_Master::burst_byte_write(uint8_t *src, size_t len)
{
    return spi_write_blocking(spi, src, len);
}

int HW_SPI_Master::burst_byte_write_read(uint8_t *src, uint8_t *dest, size_t len)
{
    return spi_write_read_blocking(spi, src, dest, len);
}

int HW_SPI_Master::single_byte_write(uint8_t byte_value)
{
    uint8_t bytes[1] = {byte_value};
    return spi_write_blocking(spi, bytes, 1);
}

int HW_SPI_Master::single_byte_read(uint8_t *dest)
{
    return spi_read_blocking(spi, 0x00, dest, 1);
}

int HW_SPI_Master::burst_byte_read(uint8_t repeated_tx_data, uint8_t *dest, size_t len)
{
    return spi_read_blocking(spi, repeated_tx_data, dest, len);
}

bool HW_SPI_Master::device_is_connected(uint8_t slave_address)
{
    return false;
}
