#include "rtos_switch_button.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"


rtosSwitchButton::rtosSwitchButton(uint gpio, struct_ConfigSwitchButton conf)
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

rtosSwitchButton::rtosSwitchButton()
{
}

rtosSwitchButton::~rtosSwitchButton()
{
}

UIControlEvent rtosSwitchButton::process_sample_event()
{
    uint32_t time_since_previous_change;
    uint32_t current_time_us = time_us_32();
    bool current_switch_pushed_state = is_switch_pushed();
    if (current_switch_pushed_state == previous_switch_pushed_state)
    {
        if (button_status == ButtonState::IDLE)
            return UIControlEvent::NONE;
        else if (button_status == ButtonState::ACTIVE)
        {
            if (current_time_us - previous_change_time_us >= long_push_delay_us)
            {
                button_status = ButtonState::RELEASE_PENDING;
                return UIControlEvent::LONG_PUSH;
            }
            else
                return UIControlEvent::NONE;
        }
        else if (button_status == ButtonState::TIME_OUT_PENDING)
        {
            if (current_time_us - previous_change_time_us >= time_out_delay_us)
            {
                button_status = ButtonState::IDLE;
                return UIControlEvent::TIME_OUT;
            }
            else
                return UIControlEvent::NONE;
        }
    }
    else
    {
        time_since_previous_change = current_time_us - previous_change_time_us;
        if (time_since_previous_change < debounce_delay_us)
            return UIControlEvent::NONE;
        else
        {
            previous_switch_pushed_state = current_switch_pushed_state;
            previous_change_time_us = current_time_us;
            if (current_switch_pushed_state)
            {
                button_status = ButtonState::ACTIVE;
                return UIControlEvent::PUSH;
            }
            else
            {
                button_status = ButtonState::TIME_OUT_PENDING;
                return (time_since_previous_change < long_release_delay_us) ? UIControlEvent::RELEASED_AFTER_SHORT_TIME : UIControlEvent::RELEASED_AFTER_LONG_TIME;
            }
        }
    }
    return UIControlEvent::NONE;
}

ButtonState rtosSwitchButton::get_button_status()
{
    return button_status;
}

bool rtosSwitchButton::is_switch_pushed()
{
    bool gpio_value = gpio_get(this->gpio);
    return ((active_lo && !gpio_value) || (!active_lo && gpio_value)) ? true : false;
}

rtosSwitchButtonWithIRQ::rtosSwitchButtonWithIRQ(uint gpio, gpio_irq_callback_t call_back, struct_ConfigSwitchButton conf, uint32_t event_mask_config)
    : rtosSwitchButton(gpio, conf)
{
    this->irq_event_mask_config = event_mask_config;
    gpio_set_irq_enabled_with_callback(gpio, irq_event_mask_config, true, call_back);
}

rtosSwitchButtonWithIRQ::rtosSwitchButtonWithIRQ()
{
}

rtosSwitchButtonWithIRQ::~rtosSwitchButtonWithIRQ()
{
}

UIControlEvent rtosSwitchButtonWithIRQ::rtos_process_IRQ_event(struct_IRQData data)
{
    bool new_switch_pushed_state = is_switch_pushed( data.event_mask);
    uint32_t current_time_us = data.current_time_us;
    uint32_t time_since_previous_change = current_time_us - previous_change_time_us;
    previous_change_time_us = current_time_us;
    if (time_since_previous_change <= debounce_delay_us)
    {
        return UIControlEvent::NONE;
    }
    else
    {
        if (new_switch_pushed_state == true)
        {
            button_status = ButtonState::ACTIVE;
            return UIControlEvent::PUSH;
        }
        else
        {
            button_status = ButtonState::TIME_OUT_PENDING;
            if (time_since_previous_change < long_release_delay_us)
                return UIControlEvent::RELEASED_AFTER_SHORT_TIME;
            else
                return UIControlEvent::RELEASED_AFTER_LONG_TIME;
        }
    }
}

void rtosSwitchButtonWithIRQ::irq_enabled(bool enabled)
{
    gpio_set_irq_enabled(this->gpio, this->irq_event_mask_config, enabled);
}

bool rtosSwitchButtonWithIRQ::is_switch_pushed(uint32_t event_mask)
{
    bool only_rising_edge_present = (event_mask & GPIO_IRQ_EDGE_RISE) and !(event_mask & GPIO_IRQ_EDGE_FALL);
    bool only_falling_edge_present = (event_mask & GPIO_IRQ_EDGE_FALL) and !(event_mask & GPIO_IRQ_EDGE_RISE);

    return ((active_lo && only_falling_edge_present) || (!active_lo && only_rising_edge_present)) ? true : false;
}
