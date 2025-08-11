/**
 * @file test_rtos_semaphore.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
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

struct config_semaphore_task
{
    Probe *probe;
    uint32_t task_duration_ms;
};

xSemaphoreHandle semaphore = xSemaphoreCreateBinary();
xSemaphoreHandle irq_SW = xSemaphoreCreateBinary();
#define uxMAX_COUNT 10
#define uxInitialCount 0
xSemaphoreHandle irq_CLK = xSemaphoreCreateCounting(uxMAX_COUNT,uxInitialCount);

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
    BaseType_t *pxSWHigherPriorityTaskWoken = pdFALSE;
    BaseType_t *pxCLKHigherPriorityTaskWoken = pdFALSE;
    if (event == GPIO_IRQ_EDGE_FALL)
    {
        switch (gpio)
        {
        case SW_GPIO_UNDER_TEST:
            xSemaphoreGiveFromISR(irq_SW, pxSWHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxSWHigherPriorityTaskWoken);
            break;
        case CLK_GPIO_UNDER_TEST:
            xSemaphoreGiveFromISR(irq_CLK, pxCLKHigherPriorityTaskWoken);
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
        xSemaphoreGive(semaphore);
}

void vIRQ_SWProcessingTask(void *pxProbe)
{
    // irq_data data;
    while (true)
    {
        xSemaphoreTake(irq_SW, portMAX_DELAY);
        ((Probe *)pxProbe)->pulse_us(400);
    }
}
void vIRQ_CLKProcessingTask(void *pxProbe)
{
    while (true)
    {
        xSemaphoreTake(irq_CLK, portMAX_DELAY);
        ((Probe *)pxProbe)->pulse_us(200);
    }
}

void vSemaphoreHandlerTask(void *pxConfiguration)
{
    const volatile config_semaphore_task *config = static_cast<config_semaphore_task *>(pxConfiguration);
    size_t count = config->task_duration_ms * 3000;
    while (true)
    {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        for (size_t i = 0; i < count; i++)
        {
            config->probe->hi();
            config->probe->lo();
        }
    }
}

int main()
{
    config_semaphore_task cnf_task = {
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
    xTaskCreate(vSemaphoreHandlerTask, "semaphore task", 256, &cnf_task, 2, NULL);
    xTaskCreate(vIRQ_SWProcessingTask, "IRQ_SW_task1", 256, &p2, 3, NULL);
    xTaskCreate(vIRQ_CLKProcessingTask, "IRQ_CLK_task1", 256, &p2, 1, NULL);
    xTimerStart(xAutoReloadTimer, 0);
    p1.pulse_us(100);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    };
}
