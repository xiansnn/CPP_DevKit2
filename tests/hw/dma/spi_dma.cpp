/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Example of writing via DMA to the SPI interface and similarly reading it back via a loopback.

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hw/spi/hw_spi.h"
#include "hardware/dma.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

#define TEST_SIZE 512

#define SPI_SCK_PIN 10
#define SPI_TX_PIN 11
#define SPI_RX_PIN 12
#define SPI_CSN_PIN 13
#define SPI_BAUD_RATE 1000 * 1000

static uint16_t txbuf[TEST_SIZE];
static uint16_t rxbuf[TEST_SIZE];

struct struct_ConfigDMA
{
    irq_num_t irq_number = DMA_IRQ_0; // can be DMQ_IRQ_1
    uint dma_channel;
    dma_channel_transfer_size dma_transfer_size = DMA_SIZE_16; // can be DMA_SIZE_8, DMA_SIZE_16, DMA_SIZE_32
    uint block_transfer_size;
    irq_handler_t dma_handler = NULL;
};

static struct_ConfigDMA dma_tx_cfg = {
    .irq_number = DMA_IRQ_0,
    .dma_transfer_size = DMA_SIZE_16,
    .block_transfer_size = TEST_SIZE};

static struct_ConfigDMA dma_rx_cfg = {
    .irq_number = DMA_IRQ_0,
    .dma_transfer_size = DMA_SIZE_16,
    .block_transfer_size = TEST_SIZE};

static struct_ConfigMasterSPI spi_cfg = {
    .spi = spi1,
    .sck_pin = SPI_SCK_PIN,
    .tx_pin = SPI_TX_PIN,
    .rx_pin = SPI_RX_PIN,
    .cs_pin = SPI_CSN_PIN,
    .baud_rate_Hz = SPI_BAUD_RATE,
    .transfer_size = 16};

void init_dma_spi(bool is_tx, uint16_t *data_buffer, struct_ConfigDMA dma_cfg, struct_ConfigMasterSPI spi_cfg)
{
    // Grab some unused dma channels
    dma_cfg.dma_channel = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_cfg.dma_channel);
    channel_config_set_transfer_data_size(&c, dma_cfg.dma_transfer_size);
    // We set the outbound DMA to transfer from a memory buffer to the SPI transmit FIFO paced by the SPI TX FIFO DREQ
    channel_config_set_dreq(&c, spi_get_dreq(spi_cfg.spi, is_tx));
    if (is_tx)
    {
        // data is sent to SPI -> the read address is incremented every transfer
        // and the write address remain unchanged.
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, false);
        dma_channel_configure(dma_cfg.dma_channel, &c,
                              &spi_get_hw(spi_cfg.spi)->dr, // write address
                              data_buffer,                  // read address
                              dma_cfg.block_transfer_size,  // element count (each element is of size transfer_data_size)
                              false);                       // don't start yet
    }
    else
    {
        // receive data from SPI -> We configure the read address to remain unchanged for each element, and inceremt the write address to memory
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, true);

        dma_channel_configure(dma_cfg.dma_channel, &c,
                              data_buffer,                  // write address
                              &spi_get_hw(spi_cfg.spi)->dr, // read address
                              dma_cfg.block_transfer_size,  // element count (each element is of size transfer_data_size)
                              false);                       // don't start yet
    }
};

int main()
{
    stdio_init_all();
    HW_SPI_Master master = HW_SPI_Master(spi_cfg);
    spi_set_format(spi_cfg.spi, spi_cfg.transfer_size,
                   spi_cfg.spi_polarity, spi_cfg.clk_phase, spi_cfg.bit_order);
    while (true)
    {
        p1.pulse_us(10);

        printf("SPI DMA example\t");

        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            txbuf[i] = i;
        }

        // printf("Configure TX DMA\n");

        //---------------------------------
        // init_dma_spi(true, txbuf, dma_tx_cfg, spi_cfg);
        //---------------
        dma_tx_cfg.dma_channel = dma_claim_unused_channel(true);
        dma_channel_config c_tx = dma_channel_get_default_config(dma_tx_cfg.dma_channel);
        channel_config_set_transfer_data_size(&c_tx, DMA_SIZE_16);
        channel_config_set_dreq(&c_tx, spi_get_dreq(spi_cfg.spi, true));
        channel_config_set_read_increment(&c_tx, true);
        channel_config_set_write_increment(&c_tx, false);
        dma_channel_configure(dma_tx_cfg.dma_channel, &c_tx,
                              &spi_get_hw(spi_cfg.spi)->dr, // write address
                              txbuf,                        // read address
                              TEST_SIZE,                    // element count (each element is of size transfer_data_size)
                              false);                       // don't start yet
        //--------------
        // printf("Configure RX DMA\n");
        //--------------
        // init_dma_spi(false, rxbuf, dma_rx_cfg, spi_cfg);
        //-----------------
        dma_rx_cfg.dma_channel = dma_claim_unused_channel(true);
        dma_channel_config c_rx = dma_channel_get_default_config(dma_rx_cfg.dma_channel);
        channel_config_set_transfer_data_size(&c_rx, DMA_SIZE_16);
        channel_config_set_dreq(&c_rx, spi_get_dreq(spi_cfg.spi, false));
        channel_config_set_read_increment(&c_rx, false);
        channel_config_set_write_increment(&c_rx, true);
        dma_channel_configure(dma_rx_cfg.dma_channel, &c_rx,
                              rxbuf,                        // write address
                              &spi_get_hw(spi_cfg.spi)->dr, // read address
                              TEST_SIZE,                    // element count (each element is of size transfer_data_size)
                              false);                       // don't start yet
        //-------------
        p3.hi();
        // printf("Starting DMAs...\n");
        // start them exactly simultaneously to avoid races (in extreme cases the FIFO could overflow)
        // dma_start_channel_mask((1u << dma_tx_cfg.dma_channel) | (1u << dma_rx_cfg.dma_channel));
        dma_channel_start(dma_rx_cfg.dma_channel);
        dma_channel_start(dma_tx_cfg.dma_channel);

        p3.lo();
        // printf("Wait for RX complete...\n");
        p6.hi();
        dma_channel_wait_for_finish_blocking(dma_rx_cfg.dma_channel);
        p6.lo();

        if (dma_channel_is_busy(dma_tx_cfg.dma_channel))
        {
            panic("RX completed before TX");
        }

        // printf("Done. Checking...");
        p3.hi();
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            if (rxbuf[i] != txbuf[i])
            {
                panic("Mismatch at %d/%d: expected %02x, got %02x",
                      i, TEST_SIZE, txbuf[i], rxbuf[i]);
            }
        }
        p3.lo();

        printf("All good\n");

        dma_channel_unclaim(dma_tx_cfg.dma_channel);
        dma_channel_unclaim(dma_rx_cfg.dma_channel);
        sleep_ms(500);
    }
    return 0;
}
