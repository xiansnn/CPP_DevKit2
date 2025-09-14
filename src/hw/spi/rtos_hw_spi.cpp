#include "hw_spi.h"
#include "rtos_hw_spi.h"

rtos_HW_SPI_Master::rtos_HW_SPI_Master(struct_ConfigMasterSPI master_config,
                                       struct_ConfigDMA tx_dma_config,
                                       struct_ConfigDMA rx_dma_config)
    : HW_SPI_Master(master_config)
{
    dma_rx = new HW_DMA(rx_dma_config);
    dma_tx = new HW_DMA(tx_dma_config);
}

rtos_HW_SPI_Master::~rtos_HW_SPI_Master()
{
    spi_deinit(spi);
    delete dma_rx;
    delete dma_tx;
}

int rtos_HW_SPI_Master::burst_write_8(uint8_t *src, size_t len)
{
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    return spi_write_blocking(spi, src, len);
}

int rtos_HW_SPI_Master::burst_write_16(uint16_t *src, size_t len)
{
    spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    return this->dma_tx->xfer_dma2spi(spi, src, len, true);
}

int rtos_HW_SPI_Master::burst_read_16(uint16_t *dst, size_t len)
{
    spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    this->dma_rx->xfer_spi2dma(spi, dst, len);
    return 0;
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

void rtos_HW_SPI_Master::spi_tx_dma_isr()
{
    if (this->dma_tx->irq_number == irq_num_t::DMA_IRQ_0)
    {
        if (dma_hw->ints0 & (1u << this->dma_tx->channel))
        {
            dma_hw->ints0 = (1u << this->dma_tx->channel); // Clear IRQ
            xSemaphoreGiveFromISR(this->dma_tx->end_of_xfer, NULL);
        }
    }
    else
    {
        if (dma_hw->ints1 & (1u << this->dma_tx->channel))
        {
            dma_hw->ints1 = (1u << this->dma_tx->channel); // Clear IRQ
            xSemaphoreGiveFromISR(this->dma_tx->end_of_xfer, NULL);
        }
    }
}

void rtos_HW_SPI_Master::spi_rx_dma_isr()
{
    if (this->dma_rx->irq_number == irq_num_t::DMA_IRQ_0)
    {
        if (dma_hw->ints0 & (1u << this->dma_rx->channel))
        {
            dma_hw->ints0 = (1u << this->dma_rx->channel); // Clear IRQ
            xSemaphoreGiveFromISR(this->dma_rx->end_of_xfer, NULL);
        }
    }
    else
    {
        if (dma_hw->ints1 & (1u << this->dma_rx->channel))
        {
            dma_hw->ints1 = (1u << this->dma_rx->channel); // Clear IRQ
            xSemaphoreGiveFromISR(this->dma_rx->end_of_xfer, NULL);
        }
    }
}
