/**
 * @file test_rtos_events.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "utilities/probe/probe.h"

#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS(500)
#define SW_GPIO_UNDER_TEST 17
#define CLK_GPIO_UNDER_TEST 21

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);

struct config_event_task
{
    Probe *probe;
    uint32_t task_duration_ms;
};

EventGroupHandle_t xEventGroup;

#define uxMAX_COUNT 10
#define uxInitialCount 0

#define mainPERIODIC_TASK_BIT (1UL << 0UL)
#define CLK_ISR_BIT (1UL << 1UL)
#define SW_ISR_BIT (1UL << 2UL)

bool status = true;

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void pico_set_led(bool led_on)
{
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
}

void gpio_callback(uint gpio, uint32_t event)
{
    p1.hi();
    BaseType_t pxSWHigherPriorityTaskWoken = pdFALSE;
    BaseType_t pxCLKHigherPriorityTaskWoken = pdFALSE;
    if (event == GPIO_IRQ_EDGE_FALL)
    {
        switch (gpio)
        {
        case SW_GPIO_UNDER_TEST:
            xEventGroupSetBitsFromISR(xEventGroup, SW_ISR_BIT, &pxSWHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxSWHigherPriorityTaskWoken);
            break;
        case CLK_GPIO_UNDER_TEST:
            xEventGroupSetBitsFromISR(xEventGroup, CLK_ISR_BIT, &pxCLKHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxCLKHigherPriorityTaskWoken);
            break;
        default:
            break;
        }
    }
    p1.lo();
}

static void prvAutoReloadTimerCallback(TimerHandle_t xTimer)
{
    pico_set_led(status);
    status = !status;
    p4.copy(status);
    if (status)
        xEventGroupSetBits(xEventGroup, mainPERIODIC_TASK_BIT);
}

void vPeriodicTask(void *pxConfiguration)
{
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = (mainPERIODIC_TASK_BIT);

    const volatile config_event_task *config = static_cast<config_event_task *>(pxConfiguration);
    size_t count = config->task_duration_ms * 3000;
    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup,
                                               xBitsToWaitFor,
                                               pdFALSE,
                                               pdFALSE,
                                               portMAX_DELAY);
        for (size_t i = 0; i < count; i++)
        {
            config->probe->hi();
            config->probe->lo();
        }
    }
}

static void vEventBitReadingTask(void *pxProbe)
{
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = (mainPERIODIC_TASK_BIT |
                                        SW_ISR_BIT |
                                        CLK_ISR_BIT);
    while (true)
    {
        xEventGroupValue = xEventGroupWaitBits(xEventGroup,
                                               xBitsToWaitFor,
                                               pdTRUE,
                                               pdFALSE,
                                               portMAX_DELAY);

        if ((xEventGroupValue & mainPERIODIC_TASK_BIT) != 0)
        {
            ((Probe *)pxProbe)->pulse_us(100);
            printf("Bit reading task -\t Event bit PERIODIC was set\n");
        }
        if ((xEventGroupValue & SW_ISR_BIT) != 0)
        {
            ((Probe *)pxProbe)->pulse_us(200);
            printf("Bit reading task -\t Event bit SW was set\n");
        }
        if ((xEventGroupValue & CLK_ISR_BIT) != 0)
        {
            ((Probe *)pxProbe)->pulse_us(400);
            printf("Bit reading task -\t Event bit CLK was set\n");
        }
    }
}

int main()
{
    stdio_init_all();
    xEventGroup = xEventGroupCreate();

    config_event_task cnf_task = {
        .probe = &p3,
        .task_duration_ms = 500};

    TimerHandle_t xAutoReloadTimer = xTimerCreate(
        "AutoReload",
        mainAUTO_RELOAD_TIMER_PERIOD,
        pdTRUE,
        0,
        prvAutoReloadTimerCallback);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_set_irq_enabled_with_callback(SW_GPIO_UNDER_TEST, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(CLK_GPIO_UNDER_TEST, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vPeriodicTask, "periodic task", 256, &cnf_task, 2, NULL);
    xTaskCreate(vEventBitReadingTask, "Bit Reader", 1000, &p2, 3, NULL);
    xTimerStart(xAutoReloadTimer, 0);
    p1.pulse_us(100);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    };
}
