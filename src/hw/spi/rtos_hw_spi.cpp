#include "hw_spi.h"
#include "rtos_hw_spi.h"

rtos_HW_SPI_Master::rtos_HW_SPI_Master(struct_ConfigMasterSPI master_config)
: HW_SPI_Master(master_config)
{
}

rtos_HW_SPI_Master::~rtos_HW_SPI_Master()
{
    spi_deinit(spi);
}

int rtos_HW_SPI_Master::burst_write_8(uint8_t *src, size_t len)
{
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    return spi_write_blocking(spi, src, len);
}

int rtos_HW_SPI_Master::burst_write_16(uint16_t *src, size_t len)
{
    spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    return spi_write16_blocking(spi, src, len);
}

int rtos_HW_SPI_Master::burst_write_read_8(uint8_t *src, uint8_t *dest, size_t len)
{
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    return spi_write_read_blocking(spi, src, dest, len);
}

int rtos_HW_SPI_Master::burst_read_8(uint8_t repeated_tx_data, uint8_t *dest, size_t len)
{
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    return spi_read_blocking(spi, repeated_tx_data, dest, len);
}