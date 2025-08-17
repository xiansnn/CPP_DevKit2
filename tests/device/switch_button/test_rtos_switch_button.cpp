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

static QueueHandle_t encoder_clk_isr_queue = xQueueCreate(5, sizeof(struct_IRQData));
static QueueHandle_t central_switch_isr_queue = xQueueCreate(5, sizeof(struct_IRQData));
static QueueHandle_t ui_control_event_queue = xQueueCreate(5, sizeof(struct_ControlEventData));

void test_switch_irq_call_back(uint gpio, uint32_t event_mask)
{
    struct_IRQData data;
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false);
    data.current_time_us = time_us_32();
    data.event_mask = event_mask;
    p1.hi();
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    switch (gpio)
    {
    case CENTRAL_SWITCH_GPIO:
        xQueueSendFromISR(central_switch_isr_queue, &data, &pxHigherPriorityTaskWoken);
        break;
    case ENCODER_CLK_GPIO:
        xQueueSendFromISR(encoder_clk_isr_queue, &data, &pxHigherPriorityTaskWoken);
        break;
    default:
        break;
    }
    portYIELD_FROM_ISR(&pxHigherPriorityTaskWoken);
    gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    p1.lo();
};

struct_ConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_ms = 5000};

struct_ConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_ms = 3000};

rtosSwitchButtonWithIRQ encoder_clk = rtosSwitchButtonWithIRQ(ENCODER_CLK_GPIO, &test_switch_irq_call_back, encoder_clk_isr_queue, ui_control_event_queue,
                                                              cfg_encoder_clk);
rtosSwitchButtonWithIRQ central_switch = rtosSwitchButtonWithIRQ(CENTRAL_SWITCH_GPIO, &test_switch_irq_call_back, central_switch_isr_queue, ui_control_event_queue,
                                                                 cfg_central_switch);

void vIdleTask(void *pxProbe)
{
    while (true)
    {
        ((Probe *)pxProbe)->hi();
        ((Probe *)pxProbe)->lo();
    }
}

void v2ndIdleTask(void * pxProbe){
    encoder_clk.test_idle_task(pxProbe);
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

void vProcessCentralSwitchIRQ(void *pxProbe)
{
    struct_IRQData sw_irq_data;
    struct_ControlEventData _event_data;
    _event_data.gpio_number = CENTRAL_SWITCH_GPIO;
    while (true)
    {
        uint time_to_wait = (central_switch.get_button_status() == ButtonState::TIME_OUT_PENDING) ? pdMS_TO_TICKS(central_switch.get_time_out_delay_ms()) : portMAX_DELAY;
        BaseType_t success = xQueueReceive(central_switch_isr_queue, &sw_irq_data, time_to_wait);
        if (success == pdFAIL)
        {
            _event_data.event = UIControlEvent::TIME_OUT;
            central_switch.set_button_status(ButtonState::IDLE);
            xQueueSend(ui_control_event_queue, &_event_data, portMAX_DELAY);
        }

        ((Probe *)pxProbe)->hi();
        central_switch.rtos_process_IRQ_event(sw_irq_data);
        ((Probe *)pxProbe)->lo();
    }
}
void vProcessEncoderCLKIRQ(void *pxProbe)
{
    struct_IRQData encoder_irq_data;
    struct_ControlEventData _event_data;
    _event_data.gpio_number = ENCODER_CLK_GPIO;
    while (true)
    {
        uint time_to_wait = (encoder_clk.get_button_status() == ButtonState::TIME_OUT_PENDING) ? pdMS_TO_TICKS(encoder_clk.get_time_out_delay_ms()) : portMAX_DELAY; // TODO extraire timeout value de chaque objet
        bool success = xQueueReceive(encoder_clk_isr_queue, &encoder_irq_data, time_to_wait);
        if (!success)
        {
            _event_data.event = UIControlEvent::TIME_OUT;
            encoder_clk.set_button_status(ButtonState::IDLE);
            xQueueSend(ui_control_event_queue, &_event_data, portMAX_DELAY);
        }

        ((Probe *)pxProbe)->hi();
        encoder_clk.rtos_process_IRQ_event(encoder_irq_data);
        ((Probe *)pxProbe)->lo();
    }
}

void vProcessControlEventTask(void *pxProbe)
{
    struct_ControlEventData data;
    while (true)
    {
        xQueueReceive(ui_control_event_queue, &data, portMAX_DELAY);
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

    xTaskCreate(v2ndIdleTask, "idle_task0", 256, &p0, 0, NULL);
    xTaskCreate(vProcessControlEventTask, "event_task0", 256, &p4, 2, NULL);
    xTaskCreate(vProcessCentralSwitchIRQ, "sw_irq_task0", 256, &p2, 4, NULL);
    xTaskCreate(vProcessEncoderCLKIRQ, "clk_irq_task0", 256, &p3, 4, NULL);

    vTaskStartScheduler();

    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}
