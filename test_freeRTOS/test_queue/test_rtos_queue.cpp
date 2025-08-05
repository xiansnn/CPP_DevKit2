/**
 * @file test_queue_debug_probe.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-07-28
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "utilities/probe/probe.h"

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);
Probe p5 = Probe(5);
Probe p6 = Probe(6);
Probe p7 = Probe(7);

struct config_periodic_task
{
    Probe *probe;
    uint32_t period_ms;
    uint32_t task_duration_ms;
};

enum class Sender
{
    DelayType,
    PulseType
};

struct delay_pulse_data
{
    Sender sender_type;
    uint8_t number;
};

// Probe *probes[] = {&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7};
// uint8_t n_probe[8] = {0, 1, 2, 3, 4, 5, 6, 7};
QueueHandle_t msg_queue = xQueueCreate(8, sizeof(uint8_t));
QueueHandle_t pulse_delay_queue = xQueueCreate(8, sizeof(delay_pulse_data));

config_periodic_task cnf_sending_task = {
    .probe = &p1,
    .period_ms = 1000,
    .task_duration_ms = 1};

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void vPeriodicSendingTask(void *pxConfig)
{
    config_periodic_task *pxConfigPeriodicTask = (config_periodic_task *)pxConfig;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    size_t count = pxConfigPeriodicTask->task_duration_ms * 2946;
    uint8_t delay;
    while (true)
    {
        for (size_t i = 0; i < count; i++)
        {
            pxConfigPeriodicTask->probe->hi();
            pxConfigPeriodicTask->probe->lo();
        };
        pxConfigPeriodicTask->probe->pulse_us(1);
        for (size_t j = 0; j < 5; j++)
        {
            delay = j + 1;
            xQueueSend(msg_queue, &delay, 0);
        }
        pxConfigPeriodicTask->probe->pulse_us(1);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(pxConfigPeriodicTask->period_ms));
    }
}

void vPeriodicSendingDelay(void *pxProbe)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    delay_pulse_data data = {
        .sender_type = Sender::DelayType,
        .number = 0};
    while (true)
    {
        ((Probe *)pxProbe)->pulse_us(1);
        for (size_t j = 3; j < 6; j++)
        {
            data.number = j;
            xQueueSend(pulse_delay_queue, &data, 0);
        }
        ((Probe *)pxProbe)->pulse_us(1);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}
void vPeriodicSendingPulse(void *pxProbe)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    delay_pulse_data data = {
        .sender_type = Sender::PulseType,
        .number = 0};
    while (true)
    {
        ((Probe *)pxProbe)->pulse_us(1);
        for (size_t j = 1; j < 3; j++)
        {
            data.number = j;
            xQueueSend(pulse_delay_queue, &data, 0);
        }
        ((Probe *)pxProbe)->pulse_us(1);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
    }
}

void vReceivingTask(void *pxProbe)
{
    uint8_t cN;
    while (true)
    {
        ((Probe *)pxProbe)->pulse_us(1);
        if (uxQueueMessagesWaiting(msg_queue) != 0)
        {
            xQueueReceive(msg_queue, &cN, pdMS_TO_TICKS(100));
            for (size_t i = 0; i < cN; i++)
            {
                ((Probe *)pxProbe)->pulse_us(10);
                sleep_us(10);
            }
        }
        ((Probe *)pxProbe)->pulse_us(1);
        vTaskDelay(2);
    }
}
void vReceivingPulseDelay(void *pxProbe)
{
    delay_pulse_data data;
    while (true)
    {
        ((Probe *)pxProbe)->pulse_us(1);
        if (uxQueueMessagesWaiting(pulse_delay_queue) != 0)
        {
            xQueueReceive(pulse_delay_queue, &data, 0);
            if (data.sender_type == Sender::DelayType)
            {
                ((Probe *)pxProbe)->pulse_us(10 * data.number);
            }
            else if (data.sender_type == Sender::PulseType)
            {
                for (size_t i = 0; i < data.number; i++)
                {
                    ((Probe *)pxProbe)->pulse_us(10);
                    sleep_us(10);
                }
            }
        }
        ((Probe *)pxProbe)->pulse_us(1);
        vTaskDelay(1);
    }
}

int main()
{
    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vPeriodicSendingTask, "sending_task1", 256, &cnf_sending_task, 3, NULL);
    xTaskCreate(vReceivingTask, "receiving_task2", 256, &p2, 2, NULL);
    xTaskCreate(vReceivingTask, "receiving_task3", 256, &p3, 2, NULL);
    xTaskCreate(vReceivingTask, "receiving_task4", 256, &p4, 2, NULL);
    xTaskCreate(vPeriodicSendingDelay, "sending_task5", 256, &p5, 3, NULL);
    xTaskCreate(vPeriodicSendingPulse, "sending_task6", 256, &p6, 3, NULL);
    xTaskCreate(vReceivingPulseDelay, "receiving_task7", 256, &p7, 2, NULL);

    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    };
}