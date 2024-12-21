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
#include "probe.h"
#include "pico/time.h"


int main()
{
    /**
     * @brief SETUP: Initialisation of a set of Probe, one for each channel.
     */

    Probe p0 = Probe(0);
    Probe p1 = Probe(1);
    Probe p2 = Probe(2);
    Probe p3 = Probe(3);
    Probe p4 = Probe(4);
    Probe p5 = Probe(5);
    Probe p6 = Probe(6);
    Probe p7 = Probe(7);

    Probe *probes[] = {&p0, &p1, &p2, &p3, &p4, &p5, &p6, &p7};

    /**
     * @brief LOOP: run an infinite loop.
     * 
     */
    while (true)
    {
        /**
         * @brief      Scan each channel and generate a pulse on the corresponding GPIO.
         */
        for (size_t i = 0; i < 8; i++)
        {
            sleep_us(100);
            probes[i]->hi();
            sleep_us(10);
            probes[i]->lo();
        }
        sleep_us(2000);
    }
    /**
     * @brief ENDLOOP:
     * 
     */
    return 0;
}