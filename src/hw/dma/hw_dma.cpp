#include "hw_dma.h"

HW_DMA::HW_DMA(irq_num_t irq_number, irq_handler_t dma_irq_handler)
{
    end_of_xfer = xSemaphoreCreateBinary();


    this->irq_number = irq_number;
    this->channel = dma_claim_unused_channel(true);
    // this->c = dma_channel_get_default_config(this->channel);
    // channel_config_set_transfer_data_size(&this->c, cfg.transfer_size);
    if (dma_irq_handler != NULL)
    {
        if (irq_number == irq_num_t::DMA_IRQ_0)
            dma_channel_set_irq0_enabled(this->channel, true);
        else
            dma_channel_set_irq1_enabled(this->channel, true);

        irq_set_exclusive_handler(this->irq_number, dma_irq_handler);
        irq_set_enabled(this->irq_number, true);
    }
}

HW_DMA::~HW_DMA()
{
    cleanup_and_free_dma_channel();
}

int HW_DMA::xfer_mem2mem(uint32_t number_of_transfer,
                         dma_channel_transfer_size_t transfer_size,
                         volatile void *write_address,
                         volatile void *read_address,
                         bool start)
{
    int error = pico_error_codes::PICO_ERROR_NONE;

    dma_channel_config c = dma_channel_get_default_config(this->channel);
    channel_config_set_transfer_data_size(&c, transfer_size);

    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);

    dma_channel_configure(this->channel,           // Channel to be configured
                          &c,                      // The configuration we just created
                          write_address,           // The initial write address
                          read_address,            // The initial read address
                          number_of_transfer, // Number of transfers; in this case each is 1 byte.
                          start);

    return error;
}

void HW_DMA::cleanup_and_free_dma_channel()
{
    dma_channel_cleanup(this->channel);
    dma_channel_unclaim(this->channel);
}

void HW_DMA::start_dma()
{
    dma_channel_start(this->channel);
}
