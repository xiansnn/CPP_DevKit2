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

void spi_rx_dma_handler();

struct struct_ConfigDMA
{
    uint channel;
    dma_channel_transfer_size transfer_size = DMA_SIZE_16; // can be DMA_SIZE_8, DMA_SIZE_16, DMA_SIZE_32
    uint block_size;                                       // number of transfer to be executed
    irq_handler_t handler = NULL;
    irq_num_t irq_number; // can be DMQ_IRQ_0 or DMA_IRQ_1
};

void start_dma(struct_ConfigDMA *dma_cfg);
void cleanup_and_free_dma_channel(struct_ConfigDMA *dma_cfg);
void write_spi2dma(struct_ConfigMasterSPI *spi_cfg, struct_ConfigDMA *dma_cfg, volatile void *write_address, bool start);
void write_dma2spi(struct_ConfigDMA *dma_cfg, struct_ConfigMasterSPI *spi_cfg, volatile void *read_address, bool start);

// void set_up_irq(struct_ConfigDMA *cfg)
// {
//     dma_channel_set_irq0_enabled(cfg->channel, true);
//     irq_set_exclusive_handler(cfg->irq_number, cfg->handler);
//     irq_set_enabled(cfg->irq_number, true);
// };

static struct_ConfigDMA dma_tx_cfg = {
    .transfer_size = DMA_SIZE_16,
    .block_size = TEST_SIZE};

static struct_ConfigDMA dma_rx_cfg = {
    .transfer_size = DMA_SIZE_16,
    .block_size = TEST_SIZE,
    .handler = spi_rx_dma_handler,
    .irq_number = DMA_IRQ_0};

static struct_ConfigMasterSPI spi_cfg = {
    .spi = spi1,
    .sck_pin = SPI_SCK_PIN,
    .tx_pin = SPI_TX_PIN,
    .rx_pin = SPI_RX_PIN,
    .cs_pin = SPI_CSN_PIN,
    .baud_rate_Hz = SPI_BAUD_RATE,
    .transfer_size = 16};

void spi_rx_dma_handler()
{
    if (dma_hw->ints0 & (1u << dma_rx_cfg.channel))
    {
        p7.hi();
        dma_hw->ints0 = (1u << dma_rx_cfg.channel); // Clear IRQ
        p7.lo();
    }
}

int main()
{
    p0.hi();
    stdio_init_all();
    HW_SPI_Master master = HW_SPI_Master(spi_cfg);
    spi_set_format(spi_cfg.spi, spi_cfg.transfer_size,
                   spi_cfg.spi_polarity, spi_cfg.clk_phase, spi_cfg.bit_order);
    p0.lo();
    while (true)
    {
        p1.hi();
        printf("SPI DMA example\t");

        // fill the TX buffer
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            txbuf[i] = i;
        }

        // printf("init write TX DMA\n");
        write_dma2spi(&dma_tx_cfg, &spi_cfg, txbuf, false);

        // printf("init write RX DMA\n");
        write_spi2dma(&spi_cfg, &dma_rx_cfg, rxbuf, false);

        // printf("Starting DMAs...\n");
        p2.hi();
        start_dma(&dma_rx_cfg);
        start_dma(&dma_tx_cfg);
        p2.lo();

        // printf("Wait for RX complete...\n");
        p3.hi();
        dma_channel_wait_for_finish_blocking(dma_rx_cfg.channel);
        p3.lo();

        // Extra test, specific for the example
        if (dma_channel_is_busy(dma_tx_cfg.channel))
        {
            panic("RX completed before TX");
        }

        // clean all and free DMA's
        cleanup_and_free_dma_channel(&dma_rx_cfg);
        cleanup_and_free_dma_channel(&dma_tx_cfg);

        // printf("Done. Checking...");
        p6.hi();
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            if (rxbuf[i] != txbuf[i])
            {
                panic("Mismatch at %d/%d: expected %02x, got %02x",
                      i, TEST_SIZE, txbuf[i], rxbuf[i]);
            }
        }
        printf("All good\n");
        p6.lo();

        p1.lo();
        sleep_ms(500);
    }
    return 0;
}

void start_dma(struct_ConfigDMA *dma_cfg)
{
    dma_channel_start(dma_cfg->channel);
}

void cleanup_and_free_dma_channel(struct_ConfigDMA *dma_cfg)
{
    dma_channel_cleanup(dma_cfg->channel);
    dma_channel_unclaim(dma_cfg->channel);
}

void write_spi2dma(struct_ConfigMasterSPI *spi_cfg, struct_ConfigDMA *dma_cfg, volatile void *write_address, bool start)
{
    dma_cfg->channel = dma_claim_unused_channel(true);
    dma_channel_config c_rx = dma_channel_get_default_config(dma_cfg->channel);
    channel_config_set_transfer_data_size(&c_rx, DMA_SIZE_16);
    channel_config_set_dreq(&c_rx, spi_get_dreq(spi_cfg->spi, false));
    channel_config_set_read_increment(&c_rx, false);
    channel_config_set_write_increment(&c_rx, true);

    if (dma_cfg->handler != NULL)
    {
        dma_channel_set_irq0_enabled(dma_cfg->channel, true);
        irq_set_exclusive_handler(dma_cfg->irq_number, dma_cfg->handler);
        irq_set_enabled(dma_cfg->irq_number, true);
    }

    dma_channel_configure(dma_cfg->channel, &c_rx,
                          write_address,                 // write address
                          &spi_get_hw(spi_cfg->spi)->dr, // read address
                          TEST_SIZE,                     // element count (each element is of size transfer_data_size)
                          start);                        // don't start yet
}

void write_dma2spi(struct_ConfigDMA *dma_cfg, struct_ConfigMasterSPI *spi_cfg, volatile void *read_address, bool start)
{
    dma_cfg->channel = dma_claim_unused_channel(true);
    dma_channel_config c_tx = dma_channel_get_default_config(dma_cfg->channel);
    channel_config_set_transfer_data_size(&c_tx, DMA_SIZE_16);
    channel_config_set_dreq(&c_tx, spi_get_dreq(spi_cfg->spi, true));
    channel_config_set_read_increment(&c_tx, true);
    channel_config_set_write_increment(&c_tx, false);

    if (dma_cfg->handler != NULL)
    {
        dma_channel_set_irq0_enabled(dma_cfg->channel, true);
        irq_set_exclusive_handler(dma_cfg->irq_number, dma_cfg->handler);
        irq_set_enabled(dma_cfg->irq_number, true);
    }

    dma_channel_configure(dma_cfg->channel, &c_tx,
                          &spi_get_hw(spi_cfg->spi)->dr, // write address
                          read_address,                  // read address
                          TEST_SIZE,                     // element count (each element is of size transfer_data_size)
                          start);                        // don't start yet
}
