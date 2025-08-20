#include "device/HC_SR04/rtos_hc_sr04.h"
#include <stdio.h>
#include "hardware/timer.h"


#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);


#define TRIG_PIN 26
#define ECHO_PIN 17
#define MEASUREMENT_PERIOD_ms 150

QueueHandle_t range_timer_queue = xQueueCreate(2, sizeof(struct_IRQData));
QueueHandle_t range_queue = xQueueCreate(2, sizeof(float));

void gpio_callback(uint gpio, uint32_t event)
{
    struct_IRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    p1.hi();
    data.event_mask = event;
    data.gpio_number = gpio;
    data.current_time_us = time_us_32();
    xQueueSendFromISR(range_timer_queue, &data, 0);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    p1.lo();
}

rtosHC_SR04 ranging_device = rtosHC_SR04(TRIG_PIN, ECHO_PIN,
                range_timer_queue,range_queue,
                gpio_callback);

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void vRangeDisplayTask(void *)
{
    float local_range;
    while (true)
    {
        xQueueReceive(range_queue, &local_range, portMAX_DELAY);
        p3.hi();
        printf("range: %.2f cm\n", local_range);
        p3.lo();
    }
}

void vRangeMeasureTask(void *)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (true)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(MEASUREMENT_PERIOD_ms));
        p2.hi();
        ranging_device.get_distance();
        p2.lo();
    }
    
}



int main()
{
    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vRangeDisplayTask, "vRangeDisplayTask", 256, NULL, 2, NULL);
    xTaskCreate(vRangeMeasureTask, "vRangeMeasureTask", 256, NULL, 4, NULL);
    
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }
}
