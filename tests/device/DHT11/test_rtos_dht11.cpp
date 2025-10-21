/**
 * @file test_rtos_dht11.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-18
 *
 * @copyright Copyright (c) 2025
 *
 */

#include <stdio.h>
#include <math.h>
#include "device/DHT11/rtos_dht11.h"
#include "utilities/probe/probe.h"
#include "timers.h"
#include "semphr.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);

#define CURRENT_DHT_PIN 16 // to be changed to the actual gpio pin
#define MAIN_TASK_PERIOD_ms 5000

static QueueHandle_t dht_reading_queue = xQueueCreate(2, sizeof(struct_DHTReading));

xSemaphoreHandle semaphore = xSemaphoreCreateBinary();

rtos_DHT11 dht = rtos_DHT11(CURRENT_DHT_PIN, dht_reading_queue);

void pico_flash_led(void *param)
{
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    uint flash_duration_ms = (uint)param;

    while (true)
    {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        p3.hi();
        vTaskDelay(pdMS_TO_TICKS(flash_duration_ms));
        p3.lo();
        gpio_put(PICO_DEFAULT_LED_PIN, false);
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

void vDisplayTask(void *)
{
    struct_DHTReading local_readings;
    while (true)
    {
        xQueueReceive(dht_reading_queue, &local_readings, portMAX_DELAY);
        p2.hi();
        if (local_readings.is_valid)
        {
            float fahrenheit = (local_readings.temp_celsius * 9 / 5) + 32;
            printf("Humidity = %.1f%%, Temperature = %.1f°C (%.1F°F)\n",
                   local_readings.humidity, local_readings.temp_celsius, fahrenheit);
        }
        else
            printf("Checksum error\n");
        p2.lo();
    }
}

static void main_task_TimerCallback(TimerHandle_t main_task_timer)
{
    p1.pulse_us(100);
    xSemaphoreGive(semaphore);
    dht.read_from_dht();
}

int main()
{
    stdio_init_all();

    TimerHandle_t main_task_timer = xTimerCreate("main task", MAIN_TASK_PERIOD_ms, pdTRUE, 0, main_task_TimerCallback);
    BaseType_t main_task_timer_Started;

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vDisplayTask, "display_task0", 256, NULL, 2, NULL);
    xTaskCreate(pico_flash_led, "led_task0", 256, (void *)10, 3, NULL);
    main_task_timer_Started = xTimerStart(main_task_timer, 0);

    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
    return 0;
}