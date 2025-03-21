#include "rotary_encoder.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>

RotaryEncoder::RotaryEncoder(uint encoder_clk_gpio, uint encoder_dt_gpio, 
                            gpio_irq_callback_t call_back,
                            control_event_processor_t event_processor,
                            struct_ConfigSwitchButton clk_conf)
    : SwitchButtonWithIRQ(encoder_clk_gpio, call_back, clk_conf)//, UIController()
{
    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;
    this->event_processor = event_processor;

    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);
}

RotaryEncoder::RotaryEncoder()
{
}

RotaryEncoder::~RotaryEncoder()
{
}

void RotaryEncoder::interrupt_service_routine(uint32_t current_irq_event_mask)
{
    irq_enabled(false);
    UIControlEvent clk_event = process_IRQ_event(current_irq_event_mask);
    if (clk_event == UIControlEvent::PUSH)
    {
        bool clockwise_rotation = gpio_get(dt_gpio);
        if (clockwise_rotation)
            event_processor(UIControlEvent::INCREMENT);
        else
            event_processor(UIControlEvent::DECREMENT);
    }
    irq_enabled(true);
}

void RotaryEncoder::update_event_processor(control_event_processor_t event_processor)
{
    this->event_processor = event_processor;
}
