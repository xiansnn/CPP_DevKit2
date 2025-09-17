/**
 * @file mem2mem.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief no benefit to use FreeRTOS for mem2me DMA -> way too fast
 * @version 0.1
 * @date 2025-09-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hw/dma/hw_dma.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p6 = Probe(6);

#define TEST_SIZE 1024

void m2m_dma_handler();

HW_DMA dma = HW_DMA(DMA_IRQ_0, m2m_dma_handler);

static uint16_t read_buf[TEST_SIZE];
static uint16_t write_buf[TEST_SIZE];

struct mem2mem_data_queue
{
    uint32_t length;
    uint16_t *source;
    uint16_t *destination;
};

QueueHandle_t m2m_data_queue = xQueueCreate(3, sizeof(mem2mem_data_queue));
SemaphoreHandle_t data_ready = xSemaphoreCreateBinary();

void m2m_dma_handler()
{
    if (dma_hw->ints0 & (1u << dma.channel))
    {
        p3.hi();
        dma_hw->ints0 = (1u << dma.channel); // Clear IRQ
        xSemaphoreGiveFromISR(dma.end_of_xfer, NULL);
        p3.lo();
    }
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
    mem2mem_data_queue data_to_send = {
        .length = TEST_SIZE,
        .source = read_buf,
        .destination = write_buf,
    };

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS((uint32_t)param));
        p1.hi();
        for (uint i = 0; i < TEST_SIZE; ++i)
            read_buf[i] = i;
        p1.lo();

        xQueueSend(m2m_data_queue, &data_to_send, portMAX_DELAY);
    }
}
void vXchange_task(void *param)
{
    mem2mem_data_queue received_data;
    while (true)
    {
        xQueueReceive(m2m_data_queue, &received_data, portMAX_DELAY);
        p2.hi();
        dma.xfer_mem2mem(received_data.length, DMA_SIZE_16,
                         received_data.destination, received_data.source, true);
        xSemaphoreTake(dma.end_of_xfer, portMAX_DELAY);
        p2.lo();
        xSemaphoreGive(data_ready);
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
            if (write_buf[i] != read_buf[i])
            {
                panic("Mismatch at %d/%d: expected %02x, got %02x",
                      i, TEST_SIZE, read_buf[i], write_buf[i]);
            }
        }
        p6.lo();
        // printf("All good\n");
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vPeriodic_data_generation_task, "compute_data", 250, (void *)500, 2, NULL);
    xTaskCreate(vXchange_task, "m2m", 250, NULL, 4, NULL);
    xTaskCreate(vChecking_task, "check_data", 250, NULL, 3, NULL);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
    return 0;
}