/**
 * @file test_probe.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief this test program generates a signal pulse on each probe channel indefinitely.
 * @version 0.1
 * @date 2023-05-02
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "FreeRTOS.h"

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

Probe *probes[] = {&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7};

void probe_sequence_0(void *)
{
    while (true)
    {
        p0.hi();
        vTaskDelay(1);
        p0.lo();
        vTaskDelay(10);
    }
}
void probe_sequence_1(void *)
{
    while (true)
    {
        p1.hi();
        vTaskDelay(1);
        p1.lo();
        vTaskDelay(20);
    }
}
void probe_sequence_2(void *)
{
    while (true)
    {
        p2.hi();
        vTaskDelay(1);
        p2.lo();
        vTaskDelay(30);
    }
}
void probe_sequence_3(void *)
{
    while (true)
    {
        p3.hi();
        vTaskDelay(1);
        p3.lo();
        vTaskDelay(40);
    }
}
void probe_sequence_4(void *)
{
    while (true)
    {
        p4.hi();
        vTaskDelay(1);
        p4.lo();
        vTaskDelay(50);
    }
}
void probe_sequence_5(void *)
{
    while (true)
    {
        p5.hi();
        vTaskDelay(1);
        p5.lo();
        vTaskDelay(60);
    }
}
void probe_sequence_6(void *)
{
    while (true)
    {
        p6.hi();
        vTaskDelay(1);
        p6.lo();
        vTaskDelay(70);
    }
}
void probe_sequence_7(void *)
{
    while (true)
    {
        p7.hi();
        vTaskDelay(1);
        p7.lo();
        vTaskDelay(80);
    }
}

void probe_sequence(void *)
{
    while (true)
    {
        for (size_t i = 2; i < 8; i++)
        {
            vTaskDelay(100);
            probes[i]->hi();
            vTaskDelay(10);
            probes[i]->lo();
        }
        vTaskDelay(1000);
    }
}

int main()
{
    // xTaskCreate(probe_sequence, "probe_sequence", 256, NULL, 1, NULL);
    xTaskCreate(probe_sequence_0, "pr_sequence_0", 256, NULL, 2, NULL);
    xTaskCreate(probe_sequence_1, "pr_sequence_1", 256, NULL, 1, NULL);
    xTaskCreate(probe_sequence_2, "pr_sequence_2", 256, NULL, 1, NULL);
    xTaskCreate(probe_sequence_3, "pr_sequence_3", 256, NULL, 1, NULL);
    xTaskCreate(probe_sequence_4, "pr_sequence_4", 256, NULL, 1, NULL);
    xTaskCreate(probe_sequence_5, "pr_sequence_5", 256, NULL, 1, NULL);
    xTaskCreate(probe_sequence_6, "pr_sequence_6", 256, NULL, 1, NULL);
    xTaskCreate(probe_sequence_7, "pr_sequence_7", 256, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    };
}