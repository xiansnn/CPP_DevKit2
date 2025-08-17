#include "rtos_switch_button.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

rtosSwitchButton::rtosSwitchButton(uint gpio, gpio_irq_callback_t call_back, QueueHandle_t in_switch_button_queue, QueueHandle_t out_control_event_queue,
                                   struct_rtosConfigSwitchButton conf, uint32_t event_mask_config)
{
    this->gpio = gpio;
    this->debounce_delay_us = conf.debounce_delay_us;
    this->long_release_delay_us = conf.long_release_delay_us;
    this->long_push_delay_ms = conf.long_push_delay_ms;
    this->time_out_delay_ms = conf.time_out_delay_ms;
    this->active_lo = conf.active_lo;

    gpio_init(this->gpio);
    if (active_lo)
        gpio_pull_up(this->gpio);
    else
        gpio_pull_down(this->gpio);
    this->previous_change_time_us = time_us_32();
    this->button_status = ButtonState::IDLE;
    this->previous_switch_pushed_state = false;

    this->switch_button_queue = in_switch_button_queue;
    this->control_event_queue = out_control_event_queue;
    this->irq_event_mask_config = event_mask_config;
    gpio_set_irq_enabled_with_callback(gpio, irq_event_mask_config, true, call_back);
}

rtosSwitchButton::~rtosSwitchButton()
{
}

void rtosSwitchButton::rtos_process_IRQ_event()
{
    struct_ControlEventData local_event_data;
    struct_IRQData local_irq_data;
    local_event_data.gpio_number = this->gpio;
    bool success;

    while (true)
    {
        if (button_status == ButtonState::ACTIVE)
        {
            success = xQueueReceive(this->switch_button_queue, &local_irq_data, pdMS_TO_TICKS(long_push_delay_ms));
            if (!success)
            {
                local_event_data.event = UIControlEvent::LONG_PUSH;
                button_status = ButtonState::RELEASE_PENDING;
                xQueueSend(this->control_event_queue, &local_event_data, portMAX_DELAY);
            }
        }
        else if (button_status == ButtonState::TIME_OUT_PENDING)
        {
            success = xQueueReceive(this->switch_button_queue, &local_irq_data, pdMS_TO_TICKS(time_out_delay_ms));
            if (!success)
            {
                local_event_data.event = UIControlEvent::TIME_OUT;
                button_status = ButtonState::IDLE;
                xQueueSend(this->control_event_queue, &local_event_data, portMAX_DELAY);
            }
        }
        else
        {
            xQueueReceive(this->switch_button_queue, &local_irq_data, portMAX_DELAY);
        }

        bool switch_was_pushed = is_switch_pushed(local_irq_data.event_mask);
        uint32_t current_time_us = local_irq_data.current_time_us;

        uint32_t time_since_previous_change = current_time_us - previous_change_time_us;
        previous_change_time_us = current_time_us;
        if (time_since_previous_change > debounce_delay_us)
        {
            if (switch_was_pushed == true)
            {
                button_status = ButtonState::ACTIVE; // button is pressed
                local_event_data.event = UIControlEvent::PUSH;
            }
            else
            {
                button_status = ButtonState::TIME_OUT_PENDING; // button is released
                if (time_since_previous_change < long_release_delay_us)
                    local_event_data.event = UIControlEvent::RELEASED_AFTER_SHORT_TIME;
                else
                    local_event_data.event = UIControlEvent::RELEASED_AFTER_LONG_TIME;
            }
            xQueueSend(this->control_event_queue, &local_event_data, portMAX_DELAY);
        }
    }
}

bool rtosSwitchButton::is_switch_pushed(uint32_t event_mask)
{
    bool only_rising_edge_present = (event_mask & GPIO_IRQ_EDGE_RISE) and !(event_mask & GPIO_IRQ_EDGE_FALL);
    bool only_falling_edge_present = (event_mask & GPIO_IRQ_EDGE_FALL) and !(event_mask & GPIO_IRQ_EDGE_RISE);

    return ((active_lo && only_falling_edge_present) || (!active_lo && only_rising_edge_present)) ? true : false;
}
