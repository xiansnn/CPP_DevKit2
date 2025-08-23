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

struct struct_ConfigDMA
{
    uint channel;
    dma_channel_transfer_size transfer_size = DMA_SIZE_16; // can be DMA_SIZE_8, DMA_SIZE_16, DMA_SIZE_32
    uint block_size;                                       // number of transfer to be executed
    irq_handler_t handler;
    irq_num_t irq_number = DMA_IRQ_0; // can be DMQ_IRQ_1
};

void m2m_dma_handler();

void init_m2m_dma(struct_ConfigDMA cfg, volatile void *write_address, volatile void *read_address);

void set_up_irq(struct_ConfigDMA cfg);

void clear_dma_channel(struct_ConfigDMA cfg);



static struct_ConfigDMA cfg_dma{
    .transfer_size = DMA_SIZE_16,
    .block_size = TEST_SIZE,
    .handler = m2m_dma_handler,
};

static uint16_t read_buf[TEST_SIZE];
static uint16_t write_buf[TEST_SIZE];

void m2m_dma_handler()
{
    if (dma_hw->ints0 & (1u << cfg_dma.channel))
    {
        p2.hi();
        dma_hw->ints0 = (1u << cfg_dma.channel); // Clear IRQ
        p2.lo();
    }
}



int main()
{
    stdio_init_all();

    for (uint i = 0; i < TEST_SIZE; ++i)
    {
        read_buf[i] = i;
    }

    while (true)
    {
        p0.hi();
        // Get a free channel, panic() if there are none
        cfg_dma.channel = dma_claim_unused_channel(true);
        printf("DMA M2M example\t");

        init_m2m_dma( cfg_dma, write_buf, read_buf);

        set_up_irq(cfg_dma);

        // Démarrer le transfert
        p1.hi();
        dma_channel_start(cfg_dma.channel);
        dma_channel_wait_for_finish_blocking(cfg_dma.channel);
        p1.lo();

        p3.hi();
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            if (write_buf[i] != read_buf[i])
            {
                panic("Mismatch at %d/%d: expected %02x, got %02x",
                      i, TEST_SIZE, read_buf[i], write_buf[i]);
            }
        }
        printf("All good\n");
        p3.lo();

        clear_dma_channel(cfg_dma);
        p0.lo();

        sleep_ms(500);
    }
}

void set_up_irq(struct_ConfigDMA cfg)
{
    dma_channel_set_irq0_enabled(cfg.channel, true);
    irq_set_exclusive_handler(cfg.irq_number, cfg_dma.handler);
    irq_set_enabled(cfg.irq_number, true);
}

void init_m2m_dma(struct_ConfigDMA cfg, volatile void * write_address, volatile void * read_address)
{
    dma_channel_config c = dma_channel_get_default_config(cfg.channel);
    channel_config_set_transfer_data_size(&c, cfg.transfer_size);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);

    dma_channel_configure(
        cfg.channel,    // Channel to be configured
        &c,                 // The configuration we just created
        write_address,              // The initial write address
        read_address,              // The initial read address
        cfg.block_size, // Number of transfers; in this case each is 1 byte.
        false               // Start immediately.
    );
}

void clear_dma_channel(struct_ConfigDMA cfg)
{
    dma_channel_cleanup(cfg.channel);
    dma_channel_unclaim(cfg.channel);
}
