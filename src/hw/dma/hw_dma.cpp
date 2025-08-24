#include "hw_dma.h"

HW_DMA::HW_DMA()
{
}

HW_DMA::HW_DMA(uint channel, struct_ConfigDMA *cfg)
{
    this->channel = channel;
    this->c = dma_channel_get_default_config(this->channel);
    channel_config_set_transfer_data_size(&this->c, cfg->transfer_size);
}

HW_DMA::~HW_DMA()
{
}

void HW_DMA::write_mem2mem(struct_ConfigDMA *cfg,
                           volatile void *write_address,
                           volatile void *read_address,
                           bool start)
{
    this->channel = dma_claim_unused_channel(true);
    this->c = dma_channel_get_default_config(this->channel);
    channel_config_set_transfer_data_size(&this->c, cfg->transfer_size);
    channel_config_set_read_increment(&this->c, true);
    channel_config_set_write_increment(&this->c, true);

    if (cfg->handler != NULL)
    {
        dma_channel_set_irq0_enabled(this->channel, true);
        irq_set_exclusive_handler(cfg->irq_number, cfg->handler);
        irq_set_enabled(cfg->irq_number, true);
    }

    dma_channel_configure(
        this->channel,   // Channel to be configured
        &c,              // The configuration we just created
        write_address,   // The initial write address
        read_address,    // The initial read address
        cfg->block_size, // Number of transfers; in this case each is 1 byte.
        start);          // Start immediately.
}

void HW_DMA::write_dma2spi(struct_ConfigDMA *dma_cfg,
                           struct_ConfigMasterSPI *spi_cfg,
                           volatile void *read_address,
                           bool start)
{
    this->channel = dma_claim_unused_channel(true);
    this->c = dma_channel_get_default_config(this->channel);
    channel_config_set_transfer_data_size(&c, dma_cfg->transfer_size);
    channel_config_set_dreq(&c, spi_get_dreq(spi_cfg->spi, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);

    if (dma_cfg->handler != NULL)
    {
        dma_channel_set_irq0_enabled(this->channel, true);
        irq_set_exclusive_handler(dma_cfg->irq_number, dma_cfg->handler);
        irq_set_enabled(dma_cfg->irq_number, true);
    }

    dma_channel_configure(this->channel, &c,
                          &spi_get_hw(spi_cfg->spi)->dr, // write address
                          read_address,                  // read address
                          dma_cfg->block_size,           // element count (each element is of size transfer_data_size)
                          start);                        // don't start yet
}

void HW_DMA::write_spi2dma(struct_ConfigMasterSPI *spi_cfg, struct_ConfigDMA *dma_cfg, volatile void *write_address, bool start)
{
    this->channel = dma_claim_unused_channel(true);
    this->c = dma_channel_get_default_config(this->channel);
    channel_config_set_transfer_data_size(&c, dma_cfg->transfer_size);
    channel_config_set_dreq(&c, spi_get_dreq(spi_cfg->spi, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);

    if (dma_cfg->handler != NULL)
    {
        dma_channel_set_irq0_enabled(this->channel, true);
        irq_set_exclusive_handler(dma_cfg->irq_number, dma_cfg->handler);
        irq_set_enabled(dma_cfg->irq_number, true);
    }

    dma_channel_configure(this->channel, &c,
                          write_address,                 // write address
                          &spi_get_hw(spi_cfg->spi)->dr, // read address
                          dma_cfg->block_size,           // element count (each element is of size transfer_data_size)
                          start);                        // don't start yet
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
