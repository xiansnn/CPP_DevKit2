/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "hw/hw_pwm.h"
#include "utilities/probe.h"


// This example drives a PWM output at a range of duty cycles, and uses
// another PWM slice in input mode to measure the duty cycle. You'll need to
// connect these two pins with a jumper wire:
const uint OUTPUT_PIN = 6; // D0
const uint MEASURE_PIN = 9; // D3
#define DIV 100
#define MEASURE_DURATION 10 // in ms

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);


const float test_duty_cycles[] = {
    0.f,
    0.1f,
    0.2f,
    0.3f,
    0.4f,
    0.5f,
    0.6f,
    0.7f,
    0.8f,
    0.9f,
    1.f};

int main()
{
    pr_D5.hi();
    stdio_init_all();
    printf("\nPWM duty cycle measurement example\n");

    // Configure PWM slice and set it running
    PWM signal_generator = PWM(OUTPUT_PIN, OUTPUT_PIN + 1, 8, 8); // STEP_ns=8, PERIOD_us=8
    signal_generator.set_width_nb_of_step(OUTPUT_PIN + 1, 100);
    signal_generator.start(true);
    PWMgatedMeasure gated_counter = PWMgatedMeasure(MEASURE_PIN, 800, 10000);

    // Note we aren't touching the other pin yet -- PWM pins are outputs by
    // default, but change to inputs once the divider mode is changed from
    // free-running. It's not wise to connect two outputs directly together!
    // gpio_set_function(OUTPUT_PIN, GPIO_FUNC_PWM);

    // For each of our test duty cycles, drive the output pin at that level,
    // and read back the actual output duty cycle using the other pin. The two
    // values should be very close!
    for (float duty_cycle : test_duty_cycles)
    {
        signal_generator.set_duty_cycle(OUTPUT_PIN, duty_cycle);
        pr_D4.hi();
        float measured_duty_cycle = gated_counter.measure_duty_cycle();
        pr_D4.lo();
        printf("Output duty cycle = %.3f%%\tmeasured input duty cycle = %.3f%%\n",
               duty_cycle * 100.f, measured_duty_cycle * 100.f);
    }
    pr_D5.lo();
    signal_generator.start(false);
}
