/**
 * @file test_rtos_event_like_notification.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-14
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

TaskHandle_t periodic_task_handle;
TaskHandle_t isr_task_handle;

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
            xTaskNotifyFromISR(isr_task_handle, SW_ISR_BIT, eSetBits, &pxSWHigherPriorityTaskWoken);
            portYIELD_FROM_ISR(pxSWHigherPriorityTaskWoken);
            break;
        case CLK_GPIO_UNDER_TEST:
            xTaskNotifyFromISR(isr_task_handle, CLK_ISR_BIT, eSetBits, &pxSWHigherPriorityTaskWoken);
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
        xTaskNotify(periodic_task_handle, mainPERIODIC_TASK_BIT, eSetBits);
}

void vPeriodicTask(void *pxConfiguration)
{
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = (mainPERIODIC_TASK_BIT);

    const volatile config_event_task *config = static_cast<config_event_task *>(pxConfiguration);
    size_t count = config->task_duration_ms * 3000;
    while (true)
    {
        xTaskNotifyWait(0, mainPERIODIC_TASK_BIT, NULL, portMAX_DELAY);
        printf("Bit notification reading task -\t notification bit mainPERIODIC_TASK_BIT was set\n");
        for (size_t i = 0; i < count; i++)
        {
            config->probe->hi();
            config->probe->lo();
        }
    }
}

static void vEventBitReadingTask(void *pxProbe)
{
    uint32_t ulNotif;
    const EventBits_t xBitsToWaitFor = (SW_ISR_BIT |
                                        CLK_ISR_BIT);
    while (true)
    {
        xTaskNotifyWait(0, xBitsToWaitFor, &ulNotif, portMAX_DELAY);

        if ((ulNotif & SW_ISR_BIT) != 0)
        {
            ((Probe *)pxProbe)->pulse_us(200);
            printf("Bit notification reading task -\t notification bit SW was set\n");
        }
        if ((ulNotif & CLK_ISR_BIT) != 0)
        {
            ((Probe *)pxProbe)->pulse_us(400);
            printf("Bit notification reading task -\t notification bit CLK was set\n");
        }
    }
}

int main()
{
    stdio_init_all();

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
    xTaskCreate(vPeriodicTask, "periodic task", 256, &cnf_task, 2, &periodic_task_handle);
    xTaskCreate(vEventBitReadingTask, "Bit Reader", 1000, &p2, 3, &isr_task_handle);
    xTimerStart(xAutoReloadTimer, 0);
    p1.pulse_us(100);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    };
}
