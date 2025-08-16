/**
 * @file test_rtos_switch_button.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <map>
#include <string>

#include "device/switch_button/rtos_switch_button.h"
#include "utilities/probe/probe.h"

#define CENTRAL_SWITCH_GPIO 17
#define ENCODER_CLK_GPIO 21

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);

static QueueHandle_t isr_queue = xQueueCreate(5, sizeof(struct_IRQData));
static QueueHandle_t control_event_queue = xQueueCreate(5, sizeof(struct_ControlEventData));

void generic_irq_call_back(uint gpio, uint32_t event_mask)
{
    struct_IRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    data.current_time_us = time_us_32();
    data.event_mask = event_mask;
    data.gpio_number = gpio;
    p1.hi();
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(isr_queue, &data, &pxHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(&pxHigherPriorityTaskWoken);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    p1.lo();
};

struct_ConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us = 4000000};

struct_ConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us = 4000000};

rtosSwitchButtonWithIRQ encoder_clk = rtosSwitchButtonWithIRQ(ENCODER_CLK_GPIO, &generic_irq_call_back, cfg_encoder_clk);
rtosSwitchButtonWithIRQ central_switch = rtosSwitchButtonWithIRQ(CENTRAL_SWITCH_GPIO, &generic_irq_call_back, cfg_central_switch);

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}
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

void vProcessIRQTask(void *pxProbe)
{
    struct_IRQData _irq_data;
    struct_ControlEventData _event_data;
    while (true)
    {
        xQueueReceive(isr_queue, &_irq_data, portMAX_DELAY);
        _event_data.gpio_number = _irq_data.gpio_number;
        switch (_irq_data.gpio_number)
        {
        case CENTRAL_SWITCH_GPIO:
            _event_data.event = central_switch.rtos_process_IRQ_event(_irq_data);
            break;
        case ENCODER_CLK_GPIO:
            _event_data.event = encoder_clk.rtos_process_IRQ_event(_irq_data);
            break;

        default:
            break;
        }
        if (_event_data.event != UIControlEvent::NONE)
        {
            ((Probe *)pxProbe)->hi();
            xQueueSend(control_event_queue, &_event_data, portMAX_DELAY);
            ((Probe *)pxProbe)->lo();
        }
        else
            p4.pulse_us(50);
    }
}

void vProcessControlEventTask(void *pxProbe)
{
    struct_ControlEventData data;
    while (true)
    {
        xQueueReceive(control_event_queue, &data, portMAX_DELAY);
        ((Probe *)pxProbe)->hi();
        switch (data.gpio_number)
        {
        case CENTRAL_SWITCH_GPIO:
            printf("CENTRAL_SWITCH_GPIO IRQ event(%s)\n", event_to_string[data.event].c_str());
            break;
        case ENCODER_CLK_GPIO:
            printf("ENCODER_CLK_GPIO IRQ event(%s) \n", event_to_string[data.event].c_str());
            break;

        default:
            break;
        }
        ((Probe *)pxProbe)->lo();
    }
}



int main()
{
    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vProcessControlEventTask, "event_task0", 256, &p3, 2, NULL);
    xTaskCreate(vProcessIRQTask, "irq_task0", 256, &p2, 4, NULL);

    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}
