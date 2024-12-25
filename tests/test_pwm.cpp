/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hw_pwm.h"

/**
 * @brief 
 * 
 * @return int 
 */
int main()
{

#define CH1 6
#define CH2 8
#define CH3 16
#define CH4 27
#define STEP_ns 1000
#define PERIOD_us 20000
#define PHASE_CORRECT true

    PWM pwm1 = PWM(CH1, CH1+1, STEP_ns, PERIOD_us, PHASE_CORRECT, false, false);
    pwm1.set_width_nb_of_step(CH1, 1);
    pwm1.set_duty_cycle(CH1+1, 0.05);

    PWM pwm2 = PWM(CH2, CH2+1, STEP_ns, PERIOD_us, PHASE_CORRECT, false, false);
    pwm2.set_width_nb_of_step(CH2, 1);
    pwm2.set_duty_cycle(CH2+1, 0.1);
    
    PWM pwm3 = PWM(CH3, CH3+1, STEP_ns, PERIOD_us, PHASE_CORRECT, false, false);
    pwm3.set_width_nb_of_step(CH3, 1);
    pwm3.set_duty_cycle(CH3+1, 0.5);
    
    PWM pwm4 = PWM(CH4, CH4+1, STEP_ns, PERIOD_us, PHASE_CORRECT, false, false);
    pwm4.set_width_nb_of_step(CH4, 1);
    pwm4.set_duty_cycle(CH4+1, 0.9);
   
    PWM::StartTogether();

}
