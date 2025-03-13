/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Fade an LED between low and high brightness. An interrupt handler updates
// the PWM slice's output level each time the counter wraps.

#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#include "hw/pwm/include/hw_pwm.h"
#include "utilities/probe/include/probe.h"

#define PWM_OUT 6
#define STEP_ns 500
#define PERIOD_us 100
#define PHASE_CORRECT false

Probe pr_D4 = Probe(4);
Probe pr_D5 = Probe(5);

PWM pwm = PWM(PWM_OUT, PWM_OUT + 1, STEP_ns, PERIOD_us, PHASE_CORRECT);

void on_pwm_wrap()
{
    pwm.clear_irq();
    static bool going_up = true;
    static float duty_cycle = 0.0;
    static float duty_cycle_step = 0.01;
    if (going_up)
    {
        duty_cycle += duty_cycle_step;
        if (duty_cycle >= 1.0)
        {
            duty_cycle = 1.0;
            going_up = false;
        }
    }
    else
    {
        duty_cycle -= duty_cycle_step;
        if (duty_cycle <= 0.0)
        {
            pr_D4.pulse_us(1);// trigger placed on the start of duty cycle = 0
            duty_cycle = 0.0;
            going_up = true;
        }
    }
    pwm.set_duty_cycle(PWM_OUT + 1, duty_cycle);
}

int main()
{
    pwm.set_width_nb_of_step(PWM_OUT, 1);
    pwm.set_duty_cycle(PWM_OUT + 1, 0.01);
    pwm.set_irq(on_pwm_wrap);
    pwm.start(true);

    while (true)
    {
        pr_D5.pulse_us(10);
        sleep_us(90);
    }
}
