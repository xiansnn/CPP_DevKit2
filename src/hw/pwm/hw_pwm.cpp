
#include "hw_pwm.h"

uint32_t PWM::slice_mask = 0x0; // initializing the class variable


PWM::PWM(uint gpio_ch_A,
         uint gpio_ch_B,
         int step_ns,
         int period_us,
         bool phase_correct,
         bool ch_A_inverted,
         bool ch_B_inverted)
{
    gpio_set_function(gpio_ch_A, GPIO_FUNC_PWM);
    gpio_set_function(gpio_ch_B, GPIO_FUNC_PWM);
    this->slice = pwm_gpio_to_slice_num(gpio_ch_A);
    pwm_config device_config = pwm_get_default_config();
    pwm_config_set_output_polarity(&device_config, ch_A_inverted, ch_B_inverted);
    pwm_config_set_phase_correct(&device_config, phase_correct);
    pwm_config_set_clkdiv_mode(&device_config, PWM_DIV_FREE_RUNNING);

    this->step_ns = step_ns;
    this->period_us = period_us;
    uint ph = phase_correct ? 2 : 1;
    uint32_t clk_sys_period_ns = 1000000000/clock_get_hz(clk_sys);
    float div = ((float)this->step_ns / (clk_sys_period_ns * ph)); 
    pwm_config_set_clkdiv(&device_config, div);
    uint16_t top = ((this->period_us * 1000) / this->step_ns) - 1;

    pwm_config_set_wrap(&device_config, top);
    pwm_init(this->slice, &device_config, false);

    PWM::slice_mask |= 0x1 << this->slice;
}

void PWM::StartTogether()
{
    pwm_set_mask_enabled(PWM::slice_mask);
}

void PWM::start(bool enabled)
{
    pwm_set_enabled(this->slice, enabled);
}

void PWM::set_width_nb_of_step(uint gpio_pin, uint16_t step_level)
{
    pwm_set_gpio_level(gpio_pin, step_level);
}

void PWM::set_duty_cycle(uint gpio_pin, float duty_cycle)
{
    uint16_t level = (duty_cycle * this->period_us * 1000) / this->step_ns;
    pwm_set_gpio_level(gpio_pin, level);
}

void PWM::set_irq(irq_handler_t handler)
{
    pwm_clear_irq(this->slice);
    pwm_set_irq_enabled(this->slice, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, handler);

    irq_set_enabled(PWM_IRQ_WRAP, true);
}

void PWM::clear_irq()
{
    pwm_clear_irq(this->slice);
}

PWMgatedMeasure::PWMgatedMeasure(uint pin_gate, uint resolution_ns, uint measure_duration_us)
{
    assert(pwm_gpio_to_channel(pin_gate) == PWM_CHAN_B);
    gpio_set_function(pin_gate, GPIO_FUNC_PWM); // ch_b
    // gpio_set_function(pin_gate-1, GPIO_FUNC_PWM); // ch_A
    this->slice = pwm_gpio_to_slice_num(pin_gate);
    pwm_config device_config = pwm_get_default_config(); // TOP default value =0xffffu  and DIV = 1
    pwm_config_set_output_polarity(&device_config, false, false);
    pwm_config_set_phase_correct(&device_config, false);
    pwm_config_set_clkdiv_mode(&device_config, PWM_DIV_B_HIGH); // counter is gated be signal on pin ch_B

    this->resolution_ns = resolution_ns;
    float min_clock_step = 1000000000 / clock_get_hz(clk_sys); // in ns
    this->measure_duration_us = measure_duration_us;
    float div = ((float)resolution_ns / min_clock_step);
    pwm_config_set_clkdiv(&device_config, div);
    pwm_init(this->slice, &device_config, false);
}

float PWMgatedMeasure::measure_duty_cycle()
{
    float max_possible_count = this->measure_duration_us*1000 / this->resolution_ns;
    return (float)this->count_cycles() / max_possible_count;
}

uint16_t PWMgatedMeasure::count_cycles()
{
    pwm_set_counter(this->slice, 0);
    pwm_set_enabled(this->slice, true);
    sleep_us(this->measure_duration_us);
    pwm_set_enabled(this->slice, false);
    return pwm_get_counter(this->slice);
}
