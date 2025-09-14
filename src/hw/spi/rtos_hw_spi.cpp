#include "hw_spi.h"
#include "rtos_hw_spi.h"

rtos_HW_SPI_Master::rtos_HW_SPI_Master(struct_ConfigMasterSPI master_config,
                                       irq_num_t dma_tx_irq_number, irq_handler_t dma_tx_irq_handler,
                                       irq_num_t dma_rx_irq_number, irq_handler_t dma_rx_irq_handler)
    : HW_SPI_Master(master_config)
{
    dma_rx = new HW_DMA(dma_rx_irq_number, dma_rx_irq_handler);
    dma_tx = new HW_DMA(dma_tx_irq_number, dma_tx_irq_handler);
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
    error_t error = pico_error_codes::PICO_ERROR_NONE;

    spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    dma_channel_config c = dma_channel_get_default_config(this->dma_rx->channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, spi_get_dreq(spi, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);

    dma_channel_configure(this->dma_tx->channel, &c,
                          &spi_get_hw(spi)->dr,
                          src,
                          len,
                          true);
    return error;
}

int rtos_HW_SPI_Master::burst_read_16(uint16_t *dst, size_t len)
{
    error_t error = pico_error_codes::PICO_ERROR_NONE;

    spi_set_format(spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    dma_channel_config c = dma_channel_get_default_config(this->dma_rx->channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, spi_get_dreq(spi, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);

    dma_channel_configure(this->dma_rx->channel, &c,
                          dst,                  // write address
                          &spi_get_hw(spi)->dr, // read address
                          len,                  // element count (each element is of size transfer_data_size)
                          true);                // immediate start
    return error;
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
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (this->dma_tx->irq_number == irq_num_t::DMA_IRQ_0)
    {
        if (dma_hw->ints0 & (1u << this->dma_tx->channel))
        {
            dma_hw->ints0 = (1u << this->dma_tx->channel); // Clear IRQ
            xSemaphoreGiveFromISR(this->dma_tx->end_of_xfer, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    else
    {
        if (dma_hw->ints1 & (1u << this->dma_tx->channel))
        {
            dma_hw->ints1 = (1u << this->dma_tx->channel); // Clear IRQ
            xSemaphoreGiveFromISR(this->dma_tx->end_of_xfer, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}

void rtos_HW_SPI_Master::spi_rx_dma_isr()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (this->dma_rx->irq_number == irq_num_t::DMA_IRQ_0)
    {
        if (dma_hw->ints0 & (1u << this->dma_rx->channel))
        {
            dma_hw->ints0 = (1u << this->dma_rx->channel); // Clear IRQ
            xSemaphoreGiveFromISR(this->dma_rx->end_of_xfer, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
    else
    {
        if (dma_hw->ints1 & (1u << this->dma_rx->channel))
        {
            dma_hw->ints1 = (1u << this->dma_rx->channel); // Clear IRQ
            xSemaphoreGiveFromISR(this->dma_rx->end_of_xfer, &xHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
