/**
 * @file switch_button.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "switch_button.h"
#include "control_event.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

SwitchButton::SwitchButton(uint gpio, struct_SwitchButtonConfig conf)
{
    this->gpio = gpio;
    this->debounce_delay_us = conf.debounce_delay_us;
    this->long_release_delay_us = conf.long_release_delay_us;
    this->long_push_delay_us = conf.long_push_delay_us;
    this->time_out_delay_us = conf.time_out_delay_us;
    this->active_lo = conf.active_lo;

    gpio_init(this->gpio);
    if (active_lo)
        gpio_pull_up(this->gpio);
    else
        gpio_pull_down(this->gpio);
    this->previous_change_time_us = time_us_32();
    this->button_status = ButtonState::IDLE;
    this->previous_switch_pushed_state = false;
}

SwitchButton::~SwitchButton()
{
}

ControlEvent SwitchButton::process_sample_event()
{
    uint32_t time_since_previous_change;
    uint32_t current_time_us = time_us_32();
    bool current_switch_pushed_state = is_switch_pushed();
    if (current_switch_pushed_state == previous_switch_pushed_state)
    {
        if (button_status == ButtonState::IDLE)
            return ControlEvent::NOOP;
        else if (button_status == ButtonState::ACTIVE)
        {
            if (current_time_us - previous_change_time_us >= long_push_delay_us)
            {
                button_status = ButtonState::RELEASE_PENDING;
                return ControlEvent::LONG_PUSH;
            }
            else
                return ControlEvent::NOOP;
        }
        else if (button_status == ButtonState::TIME_OUT_PENDING)
        {
            if (current_time_us - previous_change_time_us >= time_out_delay_us)
            {
                button_status = ButtonState::IDLE;
                return ControlEvent::TIME_OUT;
            }
            else
                return ControlEvent::NOOP;
        }
    }
    else
    {
        time_since_previous_change = current_time_us - previous_change_time_us;
        if (time_since_previous_change < debounce_delay_us)
            return ControlEvent::NOOP;
        else
        {
            previous_switch_pushed_state = current_switch_pushed_state;
            previous_change_time_us = current_time_us;
            if (current_switch_pushed_state)
            {
                button_status = ButtonState::ACTIVE;
                return ControlEvent::PUSH;
            }
            else
            {
                button_status = ButtonState::TIME_OUT_PENDING;
                return (time_since_previous_change < long_release_delay_us) ? ControlEvent::RELEASED_AFTER_SHORT_TIME : ControlEvent::RELEASED_AFTER_LONG_TIME;
            }
        }
    }
    return ControlEvent::NOOP;
}

ButtonState SwitchButton::get_button_status()
{
    return button_status;
}

bool SwitchButton::is_switch_pushed()
{
    bool gpio_value = gpio_get(this->gpio);
    return ((active_lo && !gpio_value) || (!active_lo && gpio_value)) ? true : false;
}

SwitchButtonWithIRQ::SwitchButtonWithIRQ(uint gpio, gpio_irq_callback_t call_back, struct_SwitchButtonConfig conf, uint32_t event_mask_config)
    : SwitchButton(gpio, conf)
{
    this->irq_event_mask_config = event_mask_config;
    gpio_set_irq_enabled_with_callback(gpio, irq_event_mask_config, true, call_back);
}

SwitchButtonWithIRQ::~SwitchButtonWithIRQ()
{
}

ControlEvent SwitchButtonWithIRQ::process_IRQ_event(uint32_t current_event_mask)
{
    bool new_switch_pushed_state = is_switch_pushed(current_event_mask);
    uint32_t current_time_us = time_us_32();
    uint32_t time_since_previous_change = current_time_us - previous_change_time_us;
    previous_change_time_us = current_time_us;
    if (time_since_previous_change <= debounce_delay_us)
    {
        return ControlEvent::NOOP;
    }
    else
    {
        if (new_switch_pushed_state == true)
        {
            button_status = ButtonState::ACTIVE;
            return ControlEvent::PUSH;
        }
        else
        {
            button_status = ButtonState::TIME_OUT_PENDING;
            if (time_since_previous_change < long_release_delay_us)
                return ControlEvent::RELEASED_AFTER_SHORT_TIME;
            else
                return ControlEvent::RELEASED_AFTER_LONG_TIME;
        }
    }
}

void SwitchButtonWithIRQ::irq_enabled(bool enabled)
{
    gpio_set_irq_enabled(this->gpio, this->irq_event_mask_config, enabled);
}

bool SwitchButtonWithIRQ::is_switch_pushed(uint32_t event_mask)
{
    bool only_rising_edge_present = (event_mask & GPIO_IRQ_EDGE_RISE) and !(event_mask & GPIO_IRQ_EDGE_FALL);
    bool only_falling_edge_present = (event_mask & GPIO_IRQ_EDGE_FALL) and !(event_mask & GPIO_IRQ_EDGE_RISE);

    return ((active_lo && only_falling_edge_present) || (!active_lo && only_rising_edge_present)) ? true : false;
}
