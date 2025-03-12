/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hw_PWM/hw_pwm.h"

#define CH1 6  // PWM slice 3A
#define CH2 8  // PWM slice 4A
#define CH3 16 // PWM slice 0A
#define CH4 26 // PWM slice 5A
#define STEP_ns 1000
#define PERIOD_us 20000
#define PHASE_CORRECT true

int main()
{

    PWM pwm_3 = PWM(CH1, CH1 + 1, STEP_ns, PERIOD_us, PHASE_CORRECT, false, false);
    pwm_3.set_width_nb_of_step(CH1, 1);
    pwm_3.set_duty_cycle(CH1 + 1, 0.05);

    PWM pwm_4 = PWM(CH2, CH2 + 1, STEP_ns, PERIOD_us, PHASE_CORRECT, false, false);
    pwm_4.set_width_nb_of_step(CH2, 1);
    pwm_4.set_duty_cycle(CH2 + 1, 0.1);

    PWM pwm_0 = PWM(CH3, CH3 + 1, STEP_ns, PERIOD_us, PHASE_CORRECT, false, false);
    pwm_0.set_width_nb_of_step(CH3, 1);
    pwm_0.set_duty_cycle(CH3 + 1, 0.5);

    PWM pwm_5 = PWM(CH4, CH4 + 1, STEP_ns, PERIOD_us, PHASE_CORRECT, false, false);
    pwm_5.set_width_nb_of_step(CH4, 1);
    pwm_5.set_duty_cycle(CH4 + 1, 0.9);

    PWM::StartTogether();

    while (true)
        sleep_ms(1);
}
