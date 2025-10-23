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

static QueueHandle_t encoder_clk_isr_queue = xQueueCreate(5, sizeof(struct_SwitchButtonIRQData));
static QueueHandle_t central_switch_isr_queue = xQueueCreate(5, sizeof(struct_SwitchButtonIRQData));
static QueueHandle_t ui_control_event_queue = xQueueCreate(5, sizeof(struct_ControlEventData));

void test_switch_irq_call_back(uint gpio, uint32_t event_mask)
{
    struct_SwitchButtonIRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    data.current_time_us = time_us_32();
    data.event_mask = event_mask;
    p1.hi();
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    switch (gpio)
    {
    case CENTRAL_SWITCH_GPIO:
        p2.hi();
        xQueueSendFromISR(central_switch_isr_queue, &data, &pxHigherPriorityTaskWoken);
        p2.lo();
        break;
    case ENCODER_CLK_GPIO:
        p3.hi();
        xQueueSendFromISR(encoder_clk_isr_queue, &data, &pxHigherPriorityTaskWoken);
        p3.lo();
        break;
    default:
        break;
    }
    portYIELD_FROM_ISR(&pxHigherPriorityTaskWoken);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    p1.lo();
};

struct_rtosConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1500,
    .time_out_delay_ms = 5000};

struct_rtosConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_ms = 1000,
    .time_out_delay_ms = 3000};

rtos_SwitchButton encoder_clk = rtos_SwitchButton(ENCODER_CLK_GPIO, &test_switch_irq_call_back,
                                                              encoder_clk_isr_queue, ui_control_event_queue,
                                                              cfg_encoder_clk);
rtos_SwitchButton central_switch = rtos_SwitchButton(CENTRAL_SWITCH_GPIO, &test_switch_irq_call_back,
                                                                 central_switch_isr_queue, ui_control_event_queue,
                                                                 cfg_central_switch);

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void vProcessCentralSwitchIRQ(void *)
{
    central_switch.rtos_process_IRQ_event();
}
void vProcessEncoderCLKIRQ(void *)
{
    encoder_clk.rtos_process_IRQ_event();
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

void vProcessControlEventTask(void *)
{
    struct_ControlEventData local_event_data;
    while (true)
    {
        xQueueReceive(ui_control_event_queue, &local_event_data, portMAX_DELAY);
        p4.hi();
        switch (local_event_data.gpio_number)
        {
        case CENTRAL_SWITCH_GPIO:
            printf("CENTRAL_SWITCH_GPIO IRQ event(%s)\n", event_to_string[local_event_data.event].c_str());
            break;
        case ENCODER_CLK_GPIO:
            printf("ENCODER_CLK_GPIO IRQ event(%s) \n", event_to_string[local_event_data.event].c_str());
            break;

        default:
            break;
        }
        p4.lo();
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(vIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vProcessControlEventTask, "event_task0", 256, NULL, 2, NULL);
    xTaskCreate(vProcessCentralSwitchIRQ, "sw_irq_task0", 256, NULL, 4, NULL);
    xTaskCreate(vProcessEncoderCLKIRQ, "clk_irq_task0", 256, NULL, 4, NULL);

    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}
