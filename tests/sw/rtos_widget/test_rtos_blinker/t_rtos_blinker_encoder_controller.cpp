#include "t_rtos_blinker_encoder_controller.h"

struct_rtosConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1000,
    .time_out_delay_ms = TIMEOUT_SWITCH_DELAY_ms};

struct_rtosConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1000,
    .time_out_delay_ms = TIMEOUT_SWITCH_DELAY_ms};


void central_switch_process_irq_event_task(void *)
{
    central_switch.rtos_process_IRQ_event();
}

void encoder_process_irq_event_task(void *)
{
    encoder.rtos_process_IRQ_event();
}

void ky040_encoder_irq_call_back(uint gpio, uint32_t event_mask)
{
    struct_SwitchButtonIRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    data.current_time_us = time_us_32();
    data.event_mask = event_mask;
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    switch (gpio)
    {
    case GPIO_CENTRAL_SWITCH:
        xQueueSendFromISR(central_switch.IRQdata_input_queue, &data, &pxHigherPriorityTaskWoken);
        break;
    case GPIO_ENCODER_CLK:
        xQueueSendFromISR(encoder.IRQdata_input_queue, &data, &pxHigherPriorityTaskWoken);
        break;
    default:
        break;
    }
    portYIELD_FROM_ISR(&pxHigherPriorityTaskWoken);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
};
