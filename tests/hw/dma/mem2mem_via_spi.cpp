/**
 * @file mem2mem_via_spi.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hw/spi/rtos_hw_spi.h"
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

static uint16_t rxbuf[TEST_SIZE];
static uint16_t txbuf[TEST_SIZE];
QueueHandle_t spi_tx_data_queue = xQueueCreate(8, sizeof(struct_TX_DataQueueSPI));
SemaphoreHandle_t data_ready = xSemaphoreCreateBinary();

void end_of_DMA_xfer_handler();

static struct_ConfigDMA dma_tx_cfg = {
    .transfer_size = DMA_SIZE_16,
    .block_size = TEST_SIZE,
    // .handler = end_of_DMA_xfer_handler, // TODO voir si on peur changer rx pour tx et si on peut changer IRQ
    // .irq_number = DMA_IRQ_0
};

static struct_ConfigDMA dma_rx_cfg = {
    .transfer_size = DMA_SIZE_16,
    .block_size = TEST_SIZE,
    .handler = end_of_DMA_xfer_handler,
    .irq_number = DMA_IRQ_0};

static struct_ConfigMasterSPI spi_cfg = {
    .spi = spi1,
    .sck_pin = SPI_SCK_PIN,
    .tx_pin = SPI_TX_PIN,
    .rx_pin = SPI_RX_PIN,
    .cs_pin = SPI_CSN_PIN,
    .baud_rate_Hz = SPI_BAUD_RATE,
    .transfer_size = 16};

rtos_HW_SPI_Master master = rtos_HW_SPI_Master(spi_cfg);

void end_of_DMA_xfer_handler()
{
    p7.hi();
    master.spi_rx_dma_isr();
    p7.lo();
}
void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void vPeriodic_data_generation_task(void *param)
{
    struct_TX_DataQueueSPI data_to_send;

    data_to_send.data = txbuf;
    data_to_send.length = TEST_SIZE;

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS((uint32_t)param));
        p1.hi();
        // fill the TX buffer
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            txbuf[i] = i;
        }
        xQueueSend(spi_tx_data_queue, &data_to_send, portMAX_DELAY);
        p1.lo();
    }
}
void vSpi_sending_task(void *param)
{
    struct_TX_DataQueueSPI data_to_send;
    while (true)
    {
        xQueueReceive(spi_tx_data_queue, &data_to_send, portMAX_DELAY);
        p2.hi();

        master.dma_rx->xfer_spi2dma(&spi_cfg, &dma_rx_cfg, rxbuf, true);
        master.dma_tx->xfer_dma2spi(&dma_tx_cfg, &spi_cfg, data_to_send.data, true);
        xSemaphoreTake(master.dma_rx->end_of_xfer, portMAX_DELAY);
        master.dma_tx->cleanup_and_free_dma_channel();
        master.dma_rx->cleanup_and_free_dma_channel();
        xSemaphoreGive(data_ready);
        p2.lo();
    }
}

void vChecking_task(void *param)
{
    while (true)
    {
        xSemaphoreTake(data_ready, portMAX_DELAY);
        p6.hi();
        for (uint i = 0; i < TEST_SIZE; ++i)
        {
            if (rxbuf[i] != i)
            {
                panic("Mismatch at %d/%d: expected %02x, got %02x",
                      i, TEST_SIZE, i, rxbuf[i]);
            }
        }
        p6.lo();
        printf("All good\n");
    }
}

int main()
{
    stdio_init_all();
    printf("SPI DMA example\n");

    spi_set_format(spi_cfg.spi, spi_cfg.transfer_size,
                   spi_cfg.spi_polarity, spi_cfg.clk_phase, spi_cfg.bit_order);

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vPeriodic_data_generation_task, "compute_spi_data", 250, (void *)500, 2, NULL);
    xTaskCreate(vSpi_sending_task, "send_spi_data", 250, NULL, 4, NULL);
    xTaskCreate(vChecking_task, "check_data", 250, NULL, 4, NULL);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
    return 0;
}