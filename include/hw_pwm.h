/**
 * @file hw_pwm.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#if !defined(HW_PWM_H)
#define HW_PWM_H

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

/**
 * @brief The basic PWM function
 *
 */
class PWM
{
private:
    /// @brief the hardware PWM slice of the Pico that handle this PWM signal
    uint8_t slice;
    /// @brief the minimal pulse provided by the PWM
    uint step_ns;
    /// @brief the period of the signal provided by PWM
    uint period_us;

public:
    /**
     * @brief a class variable that hold the slice number for the PWM object
     * 
     */
    static uint32_t slice_mask;

    /**
     * @brief All the defined PWM slices will be synchronised
     *
     */
    static void StartTogether();

    /**
     * @brief Construct a new PWM object
     * assuming free running  sys_clk @125 MHz => sys_clk_period = 8 ns
     *   PERIOD = 8ns * (TOP+1)*(CSR_PH_CORRECT + 1)*(DIV_INT + (DIV_FRAC/16))
     *   or
     *   PERIOD = 8ns * (TOP+1)*(CSR_PH_CORRECT + 1)* DIV) with DIV = step_ns/8
     *   TOP = (period_us * 1000 /(ph*step_ns)) -1
     * This means the minimum step_ns is 8 ns and the maximum is 256*8ns = 2.048 us
     * the maximum period is 2.048us*64536 = 132 ms
     *
     * @param gpio_ch_A channel A of the current PWM slice
     * @param gpio_ch_B channel B=A+1 of the current PWM slice. May be configured as Input
     * @param step_ns the minimal pulse provided by the PWM
     * @param period_us the period of the signal provided by PWM
     * @param phase_correct true if we want the channels symmetric.
     * @param ch_A_inverted true is we want channel A to be active LO
     * @param ch_B_inverted true is we want channel B to be active LO
     */
    PWM(uint gpio_ch_A,
        uint gpio_ch_B,
        int step_ns,   // min = PERIOD_us * 1000 / 64535 < step_ns < max = 256 * 8ns*(phase_correct+1) = ca 2us
        int period_us, // period_us max = 64535 * step_ns*(phase_correct+1)
        bool phase_correct = false,
        bool ch_A_inverted = false,
        bool ch_B_inverted = false);

    /**
     * @brief start and stop the current PWM slice
     * 
     * @param enabled 
     */
    void start(bool enabled);

    /**
     * @brief Set the pulse width in terms number of step object
     * 
     * @param gpio_pin the related pin
     * @param level the required number of steps
     */
    void set_width_nb_of_step(uint gpio_pin, uint16_t level);
    
    /**
     * @brief Set the pulse width in terms of duty cycle
     * 
     * @param gpio_pin the related pin
     * @param duty_cycle the required duty cycle
     */
    void set_duty_cycle(uint gpio_pin, float duty_cycle);
    
    /**
     * @brief Set the irq handler, executed each time the PWM counter wrap to 0.
     * 
     * @param handler 
     */
    void set_irq(irq_handler_t handler);
    
    /**
     * @brief clear the slice IRQ
     * 
     */
    void clear_irq();
};

/**
 * @brief the PWM function used to measure time during which the signal on the channel B as input is HI.
 * 
 */
class PWMgatedMeasure
{
private:
    /// @brief the hardware PWM slice of the Pico that handle this PWM signal
    uint8_t slice;
    /// @brief the operture windows during witch the PWM will count signal 
    uint measure_duration_us;
    /// @brief the minimum time slot as measure LSB
    uint resolution_ns;

public:
    
    /**
     * @brief Construct a new PWMgatedMeasure object
     * 
     * @param pin_gate must be the channel B of a slice
     * @param resolution_ns the minimum time slot as measure LSB.
     * min = 8ns < step_ns < max = 256 * 8ns = ca 2us
     * @param measure_duration_us the observation duration 
     */
    PWMgatedMeasure(uint pin_gate,
                    uint resolution_ns, 
                    uint measure_duration_us);
    /**
     * @brief compute the time the channel B is HI over the time of observation
     * 
     * @return float 
     */
    float measure_duty_cycle();
    
    /**
     * @brief gives the number of step while the signal on channel B is HI
     * 
     * @return uint16_t 
     */
    uint16_t count_cycles();
};

#endif // HW_PWM_H
