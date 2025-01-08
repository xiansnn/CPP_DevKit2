/**
 * @file test_ky040.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "ky040.h"
#include "probe.h"
#include <map>
#include <string>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21

// channel 0 : central switch pin
Probe pr_D3 = Probe(3); //
Probe pr_D4 = Probe(4); // irq_call_back is triggered
// channel 6 : encoder DT pin
// channel 7 : encoder clk pin

struct_SwitchButtonConfig cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us = 4000000};

struct_SwitchButtonConfig cfg_encoder_clk{
    .debounce_delay_us = 1000,
};

std::map<UIControlEvent, std::string> event_to_string{
    {UIControlEvent::NOOP, "NOOP"},
    {UIControlEvent::PUSH, "PUSH"},
    {UIControlEvent::DOUBLE_PUSH, "DOUBLE_PUSH"},
    {UIControlEvent::LONG_PUSH, "LONG_PUSH"},
    {UIControlEvent::RELEASED_AFTER_LONG_TIME, "RELEASED_AFTER_LONG_TIME"},
    {UIControlEvent::RELEASED_AFTER_SHORT_TIME, "RELEASED_AFTER_SHORT_TIME"},
    {UIControlEvent::INCREMENT, "INCREMENT"},
    {UIControlEvent::DECREMENT, "DECREMENT"},
    {UIControlEvent::TIME_OUT, "TIME_OUT"}};

int number_of_event = 0;

void manager_event_processor(UIControlEvent event)
{
    printf("Encoder event(%s) #%d \n", event_to_string[event].c_str(), number_of_event);
    number_of_event++;
};

void irq_call_back(uint gpio, uint32_t event_mask);

KY040 ky040 = KY040(CENTRAL_SWITCH_GPIO, ENCODER_CLK_GPIO, ENCODER_DT_GPIO, &irq_call_back, cfg_central_switch, cfg_encoder_clk);

void irq_call_back(uint gpio, uint32_t event_mask)
{
    pr_D4.pulse_us(1);
    ky040.process_encoder_IRQ(event_mask);
};

int main()
{
    stdio_init_all();

    ky040.update_UI_control_event_processor(manager_event_processor);

    while (true)
    {
        UIControlEvent central_switch_event = ky040.process_central_switch_event();
        if (central_switch_event != UIControlEvent::NOOP)
        {
            pr_D3.pulse_us(1);
            printf("central switch sampled event(%s)\n", event_to_string[central_switch_event].c_str());
        }
    }

    return 0;
}
