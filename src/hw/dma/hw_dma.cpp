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

void HW_DMA::xfer_mem2mem(struct_ConfigDMA *cfg,
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

void HW_DMA::xfer_dma2spi(struct_ConfigDMA *dma_cfg,
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

void HW_DMA::xfer_spi2dma(struct_ConfigMasterSPI *spi_cfg, struct_ConfigDMA *dma_cfg, volatile void *write_address, bool start)
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

void HW_DMA::xfer_dma2i2c(i2c_inst_t *i2c, uint8_t slave_address, uint8_t slave_mem_addr, irq_handler_t i2c_handler,
                           volatile uint8_t *read_address, size_t length, bool start)
{
    uint16_t tx_buffer[I2C_BURST_SIZE];
    size_t tx_remaining;
    size_t chunk;
    // prepare DMA for I2C TX
    this->channel = dma_claim_unused_channel(true);
    dma_channel_cleanup(channel);
    this->c = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, i2c_get_dreq(i2c, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);

    irq_num_t irq_number = (i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    if (i2c_handler != NULL)
    {
        i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_TX_EMPTY_BITS;
        irq_set_exclusive_handler(irq_number, i2c_handler);
        irq_set_enabled(irq_number, true);
    }

    // send memory address and start bit
    i2c->hw->enable = 0;
    i2c->hw->tar = slave_address;
    i2c->hw->enable = 1;
    i2c->hw->data_cmd = slave_mem_addr | I2C_IC_DATA_CMD_RESTART_BITS;

    tx_remaining = length;
    uint16_t write_data_index = 0;
    // wait for I2C TX FIFO to be empty before starting DMA
    irq_set_enabled(irq_number, true);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    // now TX FIFO is empty, start sending data via DMA in chunks of I2C_BURST_SIZE bytes
    while (tx_remaining > 0)
    {
        chunk = (tx_remaining > I2C_BURST_SIZE) ? I2C_BURST_SIZE : tx_remaining;

        for (size_t i = 0; i < chunk; i++) // fill tx_buffer to send via DMA avoiding issue with unaligned access, and to waste memory with write_data uint16_t array
        {
            tx_buffer[i] = (uint16_t)(read_address[write_data_index + i]);
            if ((write_data_index + i) == (length - 1))
                tx_buffer[i] |= I2C_IC_DATA_CMD_STOP_BITS; // add STOP condition to last byte
        }

        dma_channel_configure(channel, &c,
                              &i2c_get_hw(i2c)->data_cmd,
                              tx_buffer, // read address
                              chunk,     // element count (each element is of size transfer_data_size)
                              start);
        write_data_index += chunk;
        tx_remaining -= chunk;
        // wait for DMA to complete
        irq_set_enabled(irq_number, true);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    cleanup_and_free_dma_channel();
}

void HW_DMA::xfer_i2c2dma(i2c_inst_t *i2c, uint8_t slave_address, uint8_t slave_mem_addr, irq_handler_t i2c_handler, volatile uint8_t *dest_address, size_t length, bool start)
{
    this->channel = dma_claim_unused_channel(true);
    dma_channel_cleanup(channel);
    this->c = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(&c, i2c_get_dreq(i2c, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);

    // irq_num_t irq_number = (i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    // if (i2c_handler != NULL)
    // {
    //     i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_TX_EMPTY_BITS;
    //     irq_set_exclusive_handler(irq_number, i2c_handler);
    //     irq_set_enabled(irq_number, true);
    // }

    dma_channel_configure(channel, &c,
                          dest_address, // dma write address
                          &i2c_get_hw(i2c)->data_cmd,
                          length, // element count (each element is of size transfer_data_size)
                          start);

    cleanup_and_free_dma_channel();
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
