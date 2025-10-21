#include "rtos_rotary_encoder.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <string>

rtos_RotaryEncoder::rtos_RotaryEncoder(uint encoder_clk_gpio, uint encoder_dt_gpio,
                                     gpio_irq_callback_t call_back, QueueHandle_t in_switch_button_queue, QueueHandle_t out_control_event_queue,
                                     struct_rtosConfigSwitchButton conf, uint32_t event_mask_config)
    : rtos_SwitchButton(encoder_clk_gpio,
                       call_back, in_switch_button_queue, out_control_event_queue,
                       conf, event_mask_config)
{
    this->dt_gpio = encoder_dt_gpio;
    this->active_lo = true;

    gpio_init(this->dt_gpio);
    gpio_pull_up(this->dt_gpio);
}

rtos_RotaryEncoder::~rtos_RotaryEncoder()
{
}

void rtos_RotaryEncoder::rtos_process_IRQ_event()
{
    struct_ControlEventData local_event_data;
    struct_SwitchButtonIRQData local_irq_data;
    local_event_data.gpio_number = this->gpio;
    bool success;
    bool clockwise_rotation;
    bool switch_was_pushed;
    uint32_t current_time_us;
    uint32_t time_since_previous_change;

    while (true)
    {
        switch (button_status)
        {
        case ButtonState::TIME_OUT_PENDING:
            success = xQueueReceive(this->switch_button_queue, &local_irq_data, pdMS_TO_TICKS(time_out_delay_ms));
            if (!success)
            {
                local_event_data.event = UIControlEvent::TIME_OUT;
                button_status = ButtonState::IDLE;
                xQueueSend(this->control_event_queue, &local_event_data, portMAX_DELAY);
            }
            break;

        default:
            xQueueReceive(this->switch_button_queue, &local_irq_data, portMAX_DELAY);
            break;
        }

        switch_was_pushed = is_switch_pushed(local_irq_data.event_mask);
        current_time_us = local_irq_data.current_time_us;
        time_since_previous_change = current_time_us - previous_change_time_us;
        previous_change_time_us = current_time_us;
        if (time_since_previous_change > debounce_delay_us)
        {
            if (switch_was_pushed == true)
            {
                button_status = ButtonState::ACTIVE; // button is pressed
                clockwise_rotation = gpio_get(dt_gpio);
                if (clockwise_rotation)
                    local_event_data.event = UIControlEvent::INCREMENT;

                else
                    local_event_data.event = UIControlEvent::DECREMENT;

                xQueueSend(this->control_event_queue, &local_event_data, portMAX_DELAY);
            }
            else
                button_status = ButtonState::TIME_OUT_PENDING; // button is released
        }
    }
}
