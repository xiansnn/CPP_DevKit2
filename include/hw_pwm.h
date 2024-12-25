/**
 * @file pwm.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2023-04-21
 *
 * @copyright Copyright (c) 2023
 *
 */

#if !defined(HAL_PWM_H)
#define HAL_PWM_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

class PWM
{
private:
    uint8_t slice;
    uint step_ns;
    uint period_us;

public:
    static void StartTogether();
    static uint32_t slice_mask;

    PWM(uint gpio_ch_A,
        uint gpio_ch_B,
        int step_ns,   // min = PERIOD_us * 1000 / 64535 < step_ns < max = 256 * 8ns*(phase_correct+1) = ca 2us
        int period_us, // period_us max = 64535 * step_ns*(phase_correct+1)
        bool phase_correct = false,
        bool ch_A_inverted = false,
        bool ch_B_inverted = false);

    void start(bool enabled);
    void set_width_nb_of_step(uint gpio_pin, uint16_t level);
    void set_duty_cycle(uint gpio_pin, float duty_cycle);
    void set_irq(irq_handler_t handler);
    void clear_irq();
};

class PWMgatedMeasure
{
private:
    uint8_t slice;
    uint measure_duration_us;
    uint resolution_ns;

public:
    PWMgatedMeasure(uint pin_gate,      // must be a ch_B
                    uint resolution_ns, // min = 8ns < step_ns < max = 256 * 8ns = ca 2us
                    uint measure_duration_us);
    float measure_duty_cycle();
    uint16_t count_cycles();
};

#endif // HAL_PWM_H
