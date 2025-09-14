#include "hw_dma.h"

HW_DMA::HW_DMA(irq_num_t irq_number, irq_handler_t dma_irq_handler)
{
    end_of_xfer = xSemaphoreCreateBinary();
    TX_FIFO_empty = xSemaphoreCreateBinary(); // TODO move to I2C

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

error_t HW_DMA::xfer_dma2i2c(i2c_inst_t *i2c, uint8_t slave_address, uint8_t slave_mem_addr, irq_handler_t i2c_handler,
                             volatile uint8_t *read_address, size_t length, bool start)
{
    error_t error = pico_error_codes::PICO_ERROR_NONE;

    
    uint16_t tx_buffer[I2C_BURST_SIZE];
    size_t tx_remaining;
    size_t chunk;
    
    // prepare DMA for I2C TX
    
    this->channel = dma_claim_unused_channel(true);
    dma_channel_cleanup(channel);
    dma_channel_config c = dma_channel_get_default_config(this->channel);
    c = dma_channel_get_default_config(channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, i2c_get_dreq(i2c, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);

    this->irq_number = (i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    if (i2c_handler != NULL)
    {
        irq_set_exclusive_handler(irq_number, i2c_handler);
    }

    // send memory address and start bit
    i2c->hw->enable = 0;
    i2c->hw->tar = slave_address;
    i2c->hw->enable = 1;
    i2c->hw->data_cmd = slave_mem_addr | I2C_IC_DATA_CMD_RESTART_BITS;

    tx_remaining = length;
    uint16_t write_data_index = 0;
    while (tx_remaining > 0)
    {
        // wait for I2C TX FIFO to be empty before starting DMA
        i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_TX_EMPTY_BITS;
        irq_set_enabled(irq_number, true);
        xSemaphoreTake(this->TX_FIFO_empty, portMAX_DELAY);
        // now TX FIFO is empty, start sending data via DMA in chunks of I2C_BURST_SIZE bytes

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
    }
    // wait for end of transfer
    i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_STOP_DET_BITS;
    irq_set_enabled(irq_number, true);
    xSemaphoreTake(this->end_of_xfer, portMAX_DELAY);

    cleanup_and_free_dma_channel();
    return error;
}

error_t HW_DMA::xfer_i2c2dma(i2c_inst_t *i2c, uint8_t slave_address, uint8_t slave_mem_addr, irq_handler_t i2c_handler, volatile uint16_t *dest_address, size_t length, bool start)
{
    error_t error = pico_error_codes::PICO_ERROR_NONE;
    size_t rx_remaining;
    size_t chunk;

    this->channel = dma_claim_unused_channel(true);
    dma_channel_cleanup(channel);
    dma_channel_config c = dma_channel_get_default_config(this->channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, i2c_get_dreq(i2c, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);

    this->irq_number = (i2c == i2c0) ? I2C0_IRQ : I2C1_IRQ;
    if (i2c_handler != NULL)
    {
        irq_set_exclusive_handler(irq_number, i2c_handler);
    }
    // write command, pass mem_address
    i2c->hw->enable = 0;
    i2c->hw->tar = slave_address;
    i2c->hw->enable = 1;
    i2c->hw->data_cmd = slave_mem_addr | I2C_IC_DATA_CMD_RESTART_BITS;

    rx_remaining = length;
    size_t received_data_index = 0;
    size_t tx_cmd_index = 0;

    while (rx_remaining > 0)
    { // wait until the command is sent
        i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_TX_EMPTY_BITS;
        irq_set_enabled(irq_number, true);
        xSemaphoreTake(this->TX_FIFO_empty, portMAX_DELAY);

        chunk = (rx_remaining > I2C_BURST_SIZE) ? I2C_BURST_SIZE : rx_remaining;

        for (size_t i = 0; i < chunk; i++)
        {
            // send burst of read cmd to  TX FIFO
            uint32_t cmd = I2C_IC_DATA_CMD_CMD_BITS | (tx_cmd_index == length - 1 ? I2C_IC_DATA_CMD_STOP_BITS : 0);
            i2c->hw->data_cmd = cmd;
            tx_cmd_index++;
        }

        // read chunk of data DMA
        dma_channel_configure(channel, &c,
                              dest_address + received_data_index, // dma write address
                              &i2c_get_hw(i2c)->data_cmd,
                              length, // element count (each element is of size transfer_data_size)
                              start);
        rx_remaining -= chunk;
        received_data_index += chunk;
    }
    i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_STOP_DET_BITS;
    irq_set_enabled(irq_number, true);
    xSemaphoreTake(this->end_of_xfer, portMAX_DELAY);

    cleanup_and_free_dma_channel();
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
