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

struct_ConfigMasterSPI spi_cfg = {
    .spi = spi1,
    .sck_pin = SPI_SCK_PIN,
    .tx_pin = SPI_TX_PIN,
    .rx_pin = SPI_RX_PIN,
    .cs_pin = SPI_CSN_PIN,
    .baud_rate_Hz = SPI_BAUD_RATE,
    .transfer_size = 16};

static uint16_t txbuf[TEST_SIZE];
static uint16_t rxbuf[TEST_SIZE];
int main()
{
    stdio_init_all();
    spi_inst_t *spi = spi1;
    HW_SPI_Master master = HW_SPI_Master(spi_cfg);
    spi_set_format(spi_cfg.spi, spi_cfg.transfer_size,
                   spi_cfg.spi_polarity, spi_cfg.clk_phase, spi_cfg.bit_order);
    while (true)
    {
        p0.hi();
        printf("SPI DMA example\t");

        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            txbuf[i] = i;
        }
        p0.lo();

        // printf("Configure TX DMA\n");
        p1.hi();
        // Grab some unused dma channels
        const uint dma_tx = dma_claim_unused_channel(true);
        dma_channel_config c = dma_channel_get_default_config(dma_tx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
        // We set the outbound DMA to transfer from a memory buffer to the SPI transmit FIFO paced by the SPI TX FIFO DREQ
        // The default is for the read address to increment every element (in this case 1 byte = DMA_SIZE_8)
        // and for the write address to remain unchanged.
        channel_config_set_dreq(&c, spi_get_dreq(spi, true));
        dma_channel_configure(dma_tx, &c,
                              &spi_get_hw(spi)->dr, // write address
                              txbuf,                // read address
                              TEST_SIZE,            // element count (each element is of size transfer_data_size)
                              false);               // don't start yet
        p1.lo();

        // printf("Configure RX DMA\n");
        p2.hi();
        // Grab some unused dma channels
        const uint dma_rx = dma_claim_unused_channel(true);
        c = dma_channel_get_default_config(dma_rx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
        // We set the inbound DMA to transfer from the SPI receive FIFO to a memory buffer paced by the SPI RX FIFO DREQ
        // address to increment (so data is written throughout the buffer)
        channel_config_set_dreq(&c, spi_get_dreq(spi, false));
        // We configure the read address to remain unchanged for each element, but the write
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, true);
        dma_channel_configure(dma_rx, &c,
                              rxbuf,                // write address
                              &spi_get_hw(spi)->dr, // read address
                              TEST_SIZE,            // element count (each element is of size transfer_data_size)
                              false);               // don't start yet

        p2.lo();

        p3.hi();
        // printf("Starting DMAs...\n");
        // start them exactly simultaneously to avoid races (in extreme cases the FIFO could overflow)
        dma_start_channel_mask((1u << dma_tx) | (1u << dma_rx));
        p3.lo();
        // printf("Wait for RX complete...\n");
        p6.hi();
        dma_channel_wait_for_finish_blocking(dma_rx);
        p6.lo();

        if (dma_channel_is_busy(dma_tx))
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

        dma_channel_unclaim(dma_tx);
        dma_channel_unclaim(dma_rx);
        sleep_ms(500);
    }
    return 0;
}
