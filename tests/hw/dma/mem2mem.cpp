/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Use the DMA to copy data between two buffers in memory.

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);

#define TEST_SIZE 1024

static uint16_t txbuf[TEST_SIZE];
static uint16_t rxbuf[TEST_SIZE];

static uint dma_channel;

void dma_handler()
{
    if (dma_hw->ints0 & (1u << dma_channel))
    {
        p3.hi();
        dma_hw->ints0 = (1u << dma_channel); // Clear IRQ
        // xSemaphoreGive(dma_complete);
        p3.lo();
    }
}

// Data will be copied from src to dst
const char src[] = "Hello, world! (from DMA)";
char dst[count_of(src)];

int main()
{
    stdio_init_all();

    for (uint i = 0; i < TEST_SIZE; ++i)
    {
        txbuf[i] = i;
    }

    // 16 bit transfers. Both read and write address increment after each
    // transfer (each pointing to a location in src or dst respectively).
    // No DREQ is selected, so the DMA transfers as fast as it can.

    while (true)
    {
        p0.hi();
        // Get a free channel, panic() if there are none
        dma_channel = dma_claim_unused_channel(true);
        printf("DMA M2M example\t");

        dma_channel_config c = dma_channel_get_default_config(dma_channel);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, true);

        dma_channel_configure(
            dma_channel,      // Channel to be configured
            &c,        // The configuration we just created
            rxbuf,     // The initial write address
            txbuf,     // The initial read address
            TEST_SIZE, // Number of transfers; in this case each is 1 byte.
            false      // Start immediately.
        );

        dma_channel_set_irq0_enabled(dma_channel, true);
        irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
        irq_set_enabled(DMA_IRQ_0, true);

        // Démarrer le transfert
        dma_channel_start(dma_channel);

        // We could choose to go and do something else whilst the DMA is doing its
        // thing. In this case the processor has nothing else to do, so we just
        // wait for the DMA to finish.
        p1.hi();
        dma_channel_wait_for_finish_blocking(dma_channel);
        p1.lo();
        // The DMA has now copied our text from the transmit buffer (src) to the
        // receive buffer (dst), so we can print it out from there.
        // puts(dst);

        p2.hi();
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            if (rxbuf[i] != txbuf[i])
            {
                panic("Mismatch at %d/%d: expected %02x, got %02x",
                      i, TEST_SIZE, txbuf[i], rxbuf[i]);
            }
        }
        printf("All good\n");
        p2.lo();

        dma_channel_cleanup(dma_channel);
        dma_channel_unclaim(dma_channel);
        p0.lo();

        sleep_ms(500);
    }
}
