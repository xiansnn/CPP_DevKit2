/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
// Probe p3 = Probe(3);
// Probe p4 = Probe(4);
// Probe p5 = Probe(5);
// Probe p6 = Probe(6);
// Probe p7 = Probe(7);

struct irq_data
{
    int gpio_number;
    uint32_t event_mask;
};


#define GPIO_UNDER_TEST 17

static char event_str[128];

static const char *gpio_irq_str[] = {
    "LEVEL_LOW",  // 0x1
    "LEVEL_HIGH", // 0x2
    "EDGE_FALL",  // 0x4
    "EDGE_RISE"   // 0x8
};


QueueHandle_t event_queue = xQueueCreate(8, sizeof(irq_data));

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void gpio_event_string(char *buf, uint32_t events)
{
    for (uint i = 0; i < 4; i++)
    {
        uint mask = (1 << i);
        if (events & mask)
        {
            // Copy this event string into the user string
            const char *event_str = gpio_irq_str[i];
            while (*event_str != '\0')
            {
                *buf++ = *event_str++;
            }
            events &= ~mask;

            // If more events add ", "
            if (events)
            {
                *buf++ = ',';
                *buf++ = ' ';
            }
        }
    }
    *buf++ = '\0';
}
void gpio_callback(uint gpio, uint32_t event)
{
    p1.hi();
    irq_data data;
    data.event_mask = event;
    data.gpio_number = gpio;
    xQueueSendFromISR(event_queue, &data, 0);
    p1.lo();
}

void vIRQProcessingTask(void *pxProbe)
{
    irq_data data;
    while (true)
    {
        xQueueReceive(event_queue, &data, portMAX_DELAY);
        ((Probe *)pxProbe)->hi();
        gpio_event_string(event_str, data.event_mask);
        printf("GPIO %d %s\n", data.gpio_number, event_str);
        ((Probe *)pxProbe)->lo();

    }
}

int main()
{
    stdio_init_all();

    printf("Hello GPIO[%d] IRQ \n", GPIO_UNDER_TEST);
    gpio_set_irq_enabled_with_callback(GPIO_UNDER_TEST, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vIRQProcessingTask, "IRQ_task1", 256, &p2, 3, NULL);
    vTaskStartScheduler();

    // Wait forever
    while (true)
    {
        tight_loop_contents();
    };
}


