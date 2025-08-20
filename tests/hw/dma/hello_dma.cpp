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

// Data will be copied from src to dst
const char src[] = "Hello, world! (from DMA)";
char dst[count_of(src)];

int main()
{
    stdio_init_all();

    // Get a free channel, panic() if there are none
    int chan = dma_claim_unused_channel(true);

    // 8 bit transfers. Both read and write address increment after each
    // transfer (each pointing to a location in src or dst respectively).
    // No DREQ is selected, so the DMA transfers as fast as it can.

    while (true)
    {
        /* code */
        p0.hi();
        dma_channel_config c = dma_channel_get_default_config(chan);
        channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
        channel_config_set_read_increment(&c, true);
        channel_config_set_write_increment(&c, true);

        dma_channel_configure(
            chan,          // Channel to be configured
            &c,            // The configuration we just created
            dst,           // The initial write address
            src,           // The initial read address
            count_of(src), // Number of transfers; in this case each is 1 byte.
            true           // Start immediately.
        );

        // We could choose to go and do something else whilst the DMA is doing its
        // thing. In this case the processor has nothing else to do, so we just
        // wait for the DMA to finish.
        p1.hi();
        dma_channel_wait_for_finish_blocking(chan);
        p1.lo();
        // The DMA has now copied our text from the transmit buffer (src) to the
        // receive buffer (dst), so we can print it out from there.
        puts(dst);
        p0.lo();
        sleep_ms(100);
    }
}
