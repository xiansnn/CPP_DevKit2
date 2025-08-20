/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Example of writing via DMA to the SPI interface and similarly reading it back via a loopback.

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/dma.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);

#define TEST_SIZE 1024

/*
    .sck_pin = 10,
    .tx_pin = 11,
    .rx_pin = 12,
    .cs_pin = 13,
    .baud_rate_Hz = 10 * 1000 * 1000};
*/
#define SPI_SCK_PIN 10
#define SPI_TX_PIN 11
#define SPI_RX_PIN 12
#define SPI_CSN_PIN 13

int main()
{
    stdio_init_all();

    while (true)
    {
        p0.hi();
        printf("SPI DMA example\n");

        // Enable SPI at 1 MHz and connect to GPIOs
        spi_init(spi_default, 1000 * 1000);
        gpio_set_function(SPI_RX_PIN, GPIO_FUNC_SPI);
        gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
        gpio_set_function(SPI_SCK_PIN, GPIO_FUNC_SPI);
        gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
        // Make the SPI pins available to picotool
        bi_decl(bi_3pins_with_func(SPI_RX_PIN, SPI_TX_PIN, SPI_SCK_PIN, GPIO_FUNC_SPI));
        // Make the CS pin available to picotool
        bi_decl(bi_1pin_with_name(SPI_CSN_PIN, "SPI CS"));

        // Grab some unused dma channels
        const uint dma_tx = dma_claim_unused_channel(true);
        const uint dma_rx = dma_claim_unused_channel(true);

        // Force loopback for testing (I don't have an SPI device handy)
        hw_set_bits(&spi_get_hw(spi_default)->cr1, SPI_SSPCR1_LBM_BITS);

        static uint8_t txbuf[TEST_SIZE];
        static uint8_t rxbuf[TEST_SIZE];
        p1.hi();
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            txbuf[i] = rand();
        }
        p1.lo();
        // We set the outbound DMA to transfer from a memory buffer to the SPI transmit FIFO paced by the SPI TX FIFO DREQ
        // The default is for the read address to increment every element (in this case 1 byte = DMA_SIZE_8)
        // and for the write address to remain unchanged.

        // printf("Configure TX DMA\n");
        p2.hi();
        dma_channel_config c = dma_channel_get_default_config(dma_tx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_dreq(spi_default, true));
        dma_channel_configure(dma_tx, &c,
                              &spi_get_hw(spi_default)->dr, // write address
                              txbuf,                        // read address
                              TEST_SIZE,                    // element count (each element is of size transfer_data_size)
                              false);                       // don't start yet

        // printf("Configure RX DMA\n");
p2.lo();
        // We set the inbound DMA to transfer from the SPI receive FIFO to a memory buffer paced by the SPI RX FIFO DREQ
        // We configure the read address to remain unchanged for each element, but the write
        // address to increment (so data is written throughout the buffer)
        p3.hi();
        c = dma_channel_get_default_config(dma_rx);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_dreq(&c, spi_get_dreq(spi_default, false));
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, true);
        dma_channel_configure(dma_rx, &c,
                              rxbuf,                        // write address
                              &spi_get_hw(spi_default)->dr, // read address
                              TEST_SIZE,                    // element count (each element is of size transfer_data_size)
                              false);                       // don't start yet

        // printf("Starting DMAs...\n");
        // start them exactly simultaneously to avoid races (in extreme cases the FIFO could overflow)
        p3.lo();
        p4.hi();
        dma_start_channel_mask((1u << dma_tx) | (1u << dma_rx));
        // printf("Wait for RX complete...\n");
        dma_channel_wait_for_finish_blocking(dma_rx);
        p4.lo();
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
        p0.lo();
        sleep_ms(100);
    }
    return 0;
}
