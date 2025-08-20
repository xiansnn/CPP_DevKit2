#include "pico/stdio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "rtos_hc_sr04.h"

#define MAX_TRAVEL_TIME_ms 30

rtosHC_SR04::rtosHC_SR04(uint trig_pin, uint echo_pin,
                         QueueHandle_t input_timer_queue, QueueHandle_t output_range_queue,
                         gpio_irq_callback_t echo_irq_call_back, uint32_t echo_irq_mask_config)
{
    this->trig_pin = trig_pin;
    this->echo_pin = echo_pin;
    this->input_timer_queue = input_timer_queue;
    this->output_range_queue = output_range_queue;
    this->echo_irq_mask_config = echo_irq_mask_config;
    gpio_init(this->trig_pin);
    gpio_init(this->echo_pin);
    gpio_set_dir(this->trig_pin, GPIO_OUT);
    gpio_set_dir(this->echo_pin, GPIO_IN);
    gpio_pull_up(this->echo_pin);
    gpio_set_irq_enabled_with_callback(this->echo_pin, echo_irq_mask_config, true, echo_irq_call_back);
    this->measure_completed = false;
}

void rtosHC_SR04::get_distance()
{
    struct_HCSR04IRQData irq_data;
    float measured_range;
    uint32_t start_time_us, end_time_us;
    int32_t travel_time_us;
    bool in_range;

    measure_completed = false;

    xQueueReset(this->input_timer_queue);

    gpio_put(this->trig_pin, 1);
    sleep_us(10); // trig the device
    gpio_put(this->trig_pin, 0);


    in_range = xQueueReceive(this->input_timer_queue, &irq_data, pdMS_TO_TICKS(MAX_TRAVEL_TIME_ms));
    if (irq_data.event_mask == GPIO_IRQ_EDGE_RISE)
    {
        start_time_us = irq_data.current_time_us;
    }
    in_range = xQueueReceive(this->input_timer_queue, &irq_data, pdMS_TO_TICKS(MAX_TRAVEL_TIME_ms));
    if (irq_data.event_mask == GPIO_IRQ_EDGE_FALL)
    {
        end_time_us = irq_data.current_time_us;
        measure_completed = true;
    }
    if (measure_completed)
    {
        travel_time_us = end_time_us - start_time_us;
        if ((travel_time_us > 0) and (travel_time_us < MAX_TRAVEL_TIME_ms * 1000))
        {
            measured_range = (float)travel_time_us * 0.017; // 340m/s give 0.034 cm/us round-trip -> 0.017 cm/us
        }else
        {
            measured_range = -1.0;
        }
        xQueueSend(this->output_range_queue, &measured_range, portMAX_DELAY);
    }
}

