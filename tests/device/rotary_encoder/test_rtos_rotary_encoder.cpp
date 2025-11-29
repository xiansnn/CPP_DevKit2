/**
 * @file test_rtos_rotary_encoder.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-17
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <map>
#include <string>

#include "device/rotary_encoder/rtos_rotary_encoder.h"
#include "utilities/probe/probe.h"

#define CENTRAL_SWITCH_GPIO 18
#define ENCODER_CLK_GPIO 19
#define ENCODER_DT_GPIO 20

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);

static QueueHandle_t ky040_control_event_queue = xQueueCreate(5, sizeof(struct_ControlEventData));

std::map<UIControlEvent, std::string> event_to_string{
    {UIControlEvent::NONE, "NONE"},
    {UIControlEvent::PUSH, "PUSH"},
    {UIControlEvent::DOUBLE_PUSH, "DOUBLE_PUSH"},
    {UIControlEvent::LONG_PUSH, "LONG_PUSH"},
    {UIControlEvent::RELEASED_AFTER_LONG_TIME, "RELEASED_AFTER_LONG_TIME"},
    {UIControlEvent::RELEASED_AFTER_SHORT_TIME, "RELEASED_AFTER_SHORT_TIME"},
    {UIControlEvent::INCREMENT, "INCREMENT"},
    {UIControlEvent::DECREMENT, "DECREMENT"},
    {UIControlEvent::TIME_OUT, "TIME_OUT"}};

void ky040_irq_call_back(uint gpio, uint32_t event_mask);
//-------------------KY040 central switch-------------------------
struct_rtosConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1500,
    .time_out_delay_ms = 5000};
rtos_SwitchButton central_switch = rtos_SwitchButton(CENTRAL_SWITCH_GPIO,
                                                     &ky040_irq_call_back, ky040_control_event_queue,
                                                     cfg_central_switch);
void central_switch_process_irq_event(void *)
{
    central_switch.rtos_process_IRQ_event();
}
//--------------------KY040 rotative encoder----------------------
struct_rtosConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1000,
    .time_out_delay_ms = 3000};
rtos_RotaryEncoder encoder = rtos_RotaryEncoder(ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                                &ky040_irq_call_back, ky040_control_event_queue,
                                                cfg_encoder_clk);
void encoder_process_irq_event(void *)
{
    encoder.rtos_process_IRQ_event();
}
//--------------------ky040_irq_call_back------------------------
void ky040_irq_call_back(uint gpio, uint32_t event_mask)
{
    struct_SwitchButtonIRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    data.current_time_us = time_us_32();
    p1.hi();
    data.event_mask = event_mask;
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    switch (gpio)
    {
    case CENTRAL_SWITCH_GPIO:
        p2.hi();
        xQueueSendFromISR(central_switch.IRQdata_input_queue, &data, &pxHigherPriorityTaskWoken);
        p2.lo();
        break;
    case ENCODER_CLK_GPIO:
        p3.hi();
        xQueueSendFromISR(encoder.IRQdata_input_queue, &data, &pxHigherPriorityTaskWoken);
        p3.lo();
        break;
    default:
        break;
    }
    portYIELD_FROM_ISR(&pxHigherPriorityTaskWoken);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    p1.lo();
};

//----------------TASKS-------------------
void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

int value_inc_dec = 0;

void ky040_process_control_event(void *)
{
    struct_ControlEventData local_event_data;
    while (true)
    {
        xQueueReceive(ky040_control_event_queue, &local_event_data, portMAX_DELAY);
        switch (local_event_data.gpio_number)
        {
        case CENTRAL_SWITCH_GPIO:
            printf("CENTRAL_SWITCH_GPIO IRQ event(%s)\n", event_to_string[local_event_data.event].c_str());
            break;
        case ENCODER_CLK_GPIO:
            switch (local_event_data.event)
            {
            case UIControlEvent::INCREMENT:
                p4.pulse_us(100);
                value_inc_dec++;
                break;
            case UIControlEvent::DECREMENT:
                value_inc_dec--;
                p4.pulse_us(200);
                break;
            case UIControlEvent::TIME_OUT:
                p4.pulse_us(400);
                break;
            default:
                p4.pulse_us(1000);
                break;
            }

            printf("ENCODER_CLK_GPIO IRQ event(%s) value = %d \n", event_to_string[local_event_data.event].c_str(), value_inc_dec);
            break;
        default:
            break;
        }
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(ky040_process_control_event, "event_task0", 256, NULL, 2, NULL);
    xTaskCreate(central_switch_process_irq_event, "sw_irq_task0", 256, NULL, 4, NULL);
    xTaskCreate(encoder_process_irq_event, "clk_irq_task0", 256, NULL, 4, NULL);

    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}
