/**
 * @file mem2mem_via_spi.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief   test of SPI master/slave exchange using DMA
 * @note harware connections:
 * - connect SPI1 (as master) to SPI1 (as slave)  with:
 * - GPIO11 (MOSI) to GPIO12 (MISO)
 *  
       
 * @version 0.1
 * @date 2025-09-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hw/spi/hw_spi.h"
#include "hw/dma/hw_dma.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

#define TEST_SIZE 1024

#define SPI_SCK_PIN 10
#define SPI_TX_PIN 11
#define SPI_RX_PIN 12
#define SPI_CSN_PIN 13
#define SPI_BAUD_RATE 1000 * 1000

static uint16_t txbuf[TEST_SIZE];
static uint16_t rxbuf[TEST_SIZE];

void spi_rx_dma_handler();

static struct_ConfigDMA dma_tx_cfg = {
    .transfer_size = DMA_SIZE_16,
    .block_size = TEST_SIZE};

static struct_ConfigDMA dma_rx_cfg = {
    .transfer_size = DMA_SIZE_16,
    .block_size = TEST_SIZE,
    .handler = spi_rx_dma_handler,
    .irq_number = DMA_IRQ_0};

HW_DMA dma_rx = HW_DMA();
HW_DMA dma_tx = HW_DMA();

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
    if (dma_hw->ints0 & (1u << dma_rx.channel))
    {
        p7.hi();
        dma_hw->ints0 = (1u << dma_rx.channel); // Clear IRQ
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
        dma_tx.xfer_dma2spi(&dma_tx_cfg, &spi_cfg, txbuf, false);

        // printf("init write RX DMA\n");
        dma_rx.xfer_spi2dma(&spi_cfg, &dma_rx_cfg, rxbuf, false);

        // printf("Starting DMAs...\n");
        p2.hi();
        dma_rx.start_dma();
        dma_tx.start_dma();
        p2.lo();

        // printf("Wait for RX complete...\n");
        p3.hi();
        dma_channel_wait_for_finish_blocking(dma_rx.channel);
        p3.lo();

        // Extra test, specific for the example
        if (dma_channel_is_busy(dma_tx.channel))
        {
            panic("RX completed before TX");
        }

        // clean all and free DMA's
        dma_rx.cleanup_and_free_dma_channel();
        dma_tx.cleanup_and_free_dma_channel();

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