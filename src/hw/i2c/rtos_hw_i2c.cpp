
#include "rtos_hw_i2c.h"

#include "utilities/probe/probe.h"
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

rtos_HW_I2C_Master::rtos_HW_I2C_Master(struct_ConfigMasterI2C cfg)
    : HW_I2C_Master(cfg)
{
    this->tx_dma = new HW_DMA();
    this->i2c_master_exclusive_irq_handler = cfg.i2c_tx_master_handler;
    this->i2c_tx_FIFO_empty = xSemaphoreCreateBinary();
    this->i2c_access_mutex = xSemaphoreCreateMutex();
}

rtos_HW_I2C_Master::~rtos_HW_I2C_Master()
{
    delete tx_dma;
}

struct_I2CXferResult rtos_HW_I2C_Master::burst_byte_write(uint8_t slave_address, uint8_t mem_address, uint8_t *source_address, size_t length)
{
    xSemaphoreTake(i2c_access_mutex, portMAX_DELAY);
    struct_I2CXferResult result = {
        // TODO abort and timeout checking not implemented
        .error = false,
        .context = "rtos_burst_byte_write",
        .xfer_size = 0,
    };

    uint16_t tx_buffer[I2C_BURST_SIZE];
    size_t tx_remaining;
    size_t chunk;

    // prepare DMA for I2C TX
    dma_channel_cleanup(tx_dma->channel);
    dma_channel_config c = dma_channel_get_default_config(tx_dma->channel);
    c = dma_channel_get_default_config(tx_dma->channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, i2c_get_dreq(i2c, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);

    // send memory address and start bit
    i2c->hw->enable = 0;
    i2c->hw->tar = slave_address;
    i2c->hw->enable = 1;
    i2c->hw->data_cmd = mem_address | I2C_IC_DATA_CMD_RESTART_BITS;

    tx_remaining = length;
    uint16_t write_data_index = 0;
    while (tx_remaining > 0)
    {
        // wait for I2C TX FIFO to be empty before starting DMA
        i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_TX_EMPTY_BITS;
        irq_set_enabled(i2c_irq_number, true);
        xSemaphoreTake(this->i2c_tx_FIFO_empty, portMAX_DELAY);
        // now TX FIFO is empty, start sending data via DMA in chunks of I2C_BURST_SIZE bytes

        chunk = (tx_remaining > I2C_BURST_SIZE) ? I2C_BURST_SIZE : tx_remaining;

        for (size_t i = 0; i < chunk; i++) // fill tx_buffer to send via DMA avoiding issue with unaligned access, and to waste memory with write_data uint16_t array
        {
            tx_buffer[i] = (uint16_t)(source_address[write_data_index + i]);
            if ((write_data_index + i) == (length - 1))
                tx_buffer[i] |= I2C_IC_DATA_CMD_STOP_BITS; // add STOP condition to last byte
        }

        dma_channel_configure(tx_dma->channel, &c,
                              &i2c_get_hw(i2c)->data_cmd,
                              tx_buffer, // read address
                              chunk,     // element count (each element is of size transfer_data_size)
                              true);
        write_data_index += chunk;
        tx_remaining -= chunk;
    }
    // wait for end of transfer
    i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_STOP_DET_BITS;
    irq_set_enabled(i2c_irq_number, true);
    xSemaphoreTake(tx_dma->end_of_xfer, portMAX_DELAY);
    xSemaphoreGive(i2c_access_mutex);

    return result;
}

struct_I2CXferResult rtos_HW_I2C_Master::repeat_byte_write(uint8_t slave_address, uint8_t mem_address, uint8_t pattern, size_t length)
{
    xSemaphoreTake(i2c_access_mutex, portMAX_DELAY);
    struct_I2CXferResult result = {
        // TODO abort and timeout checking not implemented
        .error = false,
        .context = "rtos_repeat_byte_write",
        .xfer_size = 0,
    };

    uint16_t tx_buffer[I2C_BURST_SIZE];
    size_t tx_remaining;
    size_t chunk;

    // prepare DMA for I2C TX
    dma_channel_cleanup(tx_dma->channel);
    dma_channel_config c = dma_channel_get_default_config(tx_dma->channel);
    c = dma_channel_get_default_config(tx_dma->channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, i2c_get_dreq(i2c, true));
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);

    // send memory address and start bit
    i2c->hw->enable = 0;
    i2c->hw->tar = slave_address;
    i2c->hw->enable = 1;
    i2c->hw->data_cmd = mem_address | I2C_IC_DATA_CMD_RESTART_BITS;

    tx_remaining = length;
    uint16_t write_data_index = 0;
    while (tx_remaining > 0)
    {
        // wait for I2C TX FIFO to be empty before starting DMA
        i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_TX_EMPTY_BITS;
        irq_set_enabled(i2c_irq_number, true);
        xSemaphoreTake(this->i2c_tx_FIFO_empty, portMAX_DELAY);
        // now TX FIFO is empty, start sending data via DMA in chunks of I2C_BURST_SIZE bytes

        chunk = (tx_remaining > I2C_BURST_SIZE) ? I2C_BURST_SIZE : tx_remaining;

        for (size_t i = 0; i < chunk; i++) // fill tx_buffer to send via DMA avoiding issue with unaligned access, and to waste memory with write_data uint16_t array
        {
            tx_buffer[i] = (uint16_t)pattern;
            if ((write_data_index + i) == (length - 1))
                tx_buffer[i] |= I2C_IC_DATA_CMD_STOP_BITS; // add STOP condition to last byte
        }

        dma_channel_configure(tx_dma->channel, &c,
                              &i2c_get_hw(i2c)->data_cmd,
                              tx_buffer, // read address
                              chunk,     // element count (each element is of size transfer_data_size)
                              true);
        write_data_index += chunk;
        tx_remaining -= chunk;
    }
    // wait for end of transfer
    i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_STOP_DET_BITS;
    irq_set_enabled(i2c_irq_number, true);
    xSemaphoreTake(tx_dma->end_of_xfer, portMAX_DELAY);
    xSemaphoreGive(i2c_access_mutex);

    return result;
}

struct_I2CXferResult rtos_HW_I2C_Master::burst_byte_read(uint8_t slave_address,
                                                         uint8_t mem_address,
                                                         uint8_t *destination_address,
                                                         size_t length)
{
    xSemaphoreTake(i2c_access_mutex, portMAX_DELAY);
    struct_I2CXferResult result = {
        // TODO abort and timeout checking not implemented
        .error = false,
        .context = "rtos_burst_byte_write",
        .xfer_size = 0,
    };

    uint16_t *destination_buffer = new uint16_t[length];
    error_t error_code = pico_error_codes::PICO_ERROR_NONE; // TODO abort and timeout checking not implemented
    size_t rx_remaining;
    size_t chunk;

    dma_channel_cleanup(tx_dma->channel);
    dma_channel_config c = dma_channel_get_default_config(tx_dma->channel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_dreq(&c, i2c_get_dreq(i2c, false));
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);

    // write command, pass mem_address
    i2c->hw->enable = 0;
    i2c->hw->tar = slave_address;
    i2c->hw->enable = 1;
    i2c->hw->data_cmd = mem_address | I2C_IC_DATA_CMD_RESTART_BITS;

    rx_remaining = length;
    size_t received_data_index = 0;
    size_t tx_cmd_index = 0;

    while (rx_remaining > 0)
    { // wait until the command is sent
        i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_TX_EMPTY_BITS;
        irq_set_enabled(i2c_irq_number, true);
        xSemaphoreTake(this->i2c_tx_FIFO_empty, portMAX_DELAY);

        chunk = (rx_remaining > I2C_BURST_SIZE) ? I2C_BURST_SIZE : rx_remaining;

        for (size_t i = 0; i < chunk; i++)
        {
            // send burst of read cmd to  TX FIFO
            uint32_t cmd = I2C_IC_DATA_CMD_CMD_BITS | (tx_cmd_index == length - 1 ? I2C_IC_DATA_CMD_STOP_BITS : 0);
            i2c->hw->data_cmd = cmd;
            tx_cmd_index++;
        }

        // read chunk of data DMA
        dma_channel_configure(tx_dma->channel, &c,
                              destination_buffer + received_data_index, // dma write address
                              &i2c_get_hw(i2c)->data_cmd,
                              length, // element count (each element is of size transfer_data_size)
                              true);
        rx_remaining -= chunk;
        received_data_index += chunk;
    }
    i2c->hw->intr_mask = I2C_IC_INTR_STAT_R_STOP_DET_BITS;
    irq_set_enabled(i2c_irq_number, true);
    xSemaphoreTake(tx_dma->end_of_xfer, portMAX_DELAY);
    // reformat destination data from 16 to 8 bit
    for (size_t i = 0; i < length; i++)
    {
        destination_address[i] = (uint8_t)destination_buffer[i];
    }

    delete destination_buffer;
    xSemaphoreGive(i2c_access_mutex);

    return result;
}

void rtos_HW_I2C_Master::i2c_dma_isr()
{
    p5.hi();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    irq_set_enabled(this->i2c_irq_number, false); // disable IRQs to avoid re-entrance
    if (this->i2c->hw->intr_stat & I2C_IC_INTR_STAT_R_STOP_DET_BITS)
    {
        p7.hi();
        this->i2c->hw->clr_stop_det;                 // clear the STOP_DET interrupt
        irq_set_enabled(this->i2c_irq_number, true); // enable IRQs
        xSemaphoreGiveFromISR(this->tx_dma->end_of_xfer, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        p7.lo();
    }
    if (this->i2c->hw->intr_stat & I2C_IC_INTR_STAT_R_TX_EMPTY_BITS)
    {
        p6.hi();
        xSemaphoreGiveFromISR(this->i2c_tx_FIFO_empty, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        p6.lo();
    }
    p5.lo();
}
