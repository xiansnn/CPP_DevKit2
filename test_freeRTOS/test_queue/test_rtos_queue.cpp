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
    int probe_number;
    uint32_t period;
    uint32_t task_duration_ms;
};

Probe *probes[] = {&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7};
int n_delay[8] = {0, 1, 2, 3, 4, 5, 6, 7};

config_periodic_task cnf_task = {
    .probe_number = 1,
    .period = 250,
    .task_duration_ms = 5};

void probe_sequence_full_load(void *pxDelay)
{
    volatile int *delay = static_cast<int *>(pxDelay);
    while (true)
    {
        probes[*delay]->hi();
        probes[*delay]->lo();
    }
}
// void probe_sequence_delay(void *pxDelay)
// {
//     volatile int *delay = static_cast<int *>(pxDelay);
//     while (true)
//     {
//         probes[*delay]->hi();
//         vTaskDelay(1);
//         probes[*delay]->lo();
//         vTaskDelay((*delay + 1) * 10);
//     }
// }
void probe_sequence_periodic(void *pxPeriod)
{
    const volatile config_periodic_task *config = static_cast<config_periodic_task *>(pxPeriod);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    size_t count = config->task_duration_ms * 2946;
    while (true)
    {
        for (size_t i = 0; i < count; i++)
        {
            probes[config->probe_number]->hi();
            probes[config->probe_number]->lo();
        }
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(config->period));
    }
}

int main()
{

    xTaskCreate(probe_sequence_full_load, "idle_task", 256, &n_delay[0], 0, NULL);
    xTaskCreate(probe_sequence_periodic, "periodic_task", 250, &cnf_task, 1, NULL);
    // xTaskCreate(probe_sequence_full_load, "pr_sequence_01", 256, &n_delay[1], 1, NULL);
    // xTaskCreate(probe_sequence_delay, "pr_sequence_12", 256, &n_delay[2], 2, NULL);
    // xTaskCreate(probe_sequence_delay, "pr_sequence_13", 256, &n_delay[3], 2, NULL);
    // xTaskCreate(probe_sequence_delay, "pr_sequence_14", 256, &n_delay[4], 2, NULL);
    // xTaskCreate(probe_sequence_delay, "pr_sequence_15", 256, &n_delay[5], 2, NULL);
    // xTaskCreate(probe_sequence_delay, "pr_sequence_16", 256, &n_delay[6], 2, NULL);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    };
}