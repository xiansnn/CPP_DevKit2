/**
 * @file test_switch_button_irq.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "switch_button.h"
#include "control_event.h"
#include "probe.h"
#include <map>
#include <string>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26

// channel 0 : central switch pin
Probe pr_D3 = Probe(3); // sw_call_back is running
Probe pr_D4 = Probe(4); // clk_event != SwitchButtonEvent::NOOP
Probe pr_D5 = Probe(5); // bounces discarded
// channel 6 : encoder DT pin
// channel 7 : encoder clk pin

struct_SwitchButtonConfig cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us=4000000};

struct_SwitchButtonConfig cfg_encoder_clk{
    .debounce_delay_us = 1000,
};

std::map<ControlEvent, std::string> event_to_string{
    {ControlEvent::NOOP, "NOOP"},
    {ControlEvent::PUSH, "PUSH"},
    {ControlEvent::DOUBLE_PUSH, "DOUBLE_PUSH"},
    {ControlEvent::LONG_PUSH, "LONG_PUSH"},
    {ControlEvent::RELEASED_AFTER_LONG_TIME, "RELEASED_AFTER_LONG_TIME"},
    {ControlEvent::RELEASED_AFTER_SHORT_TIME, "RELEASED_AFTER_SHORT_TIME"},
    {ControlEvent::INCREMENT, "INCREMENT"},
    {ControlEvent::DECREMENT, "DECREMENT"},
    {ControlEvent::TIME_OUT, "TIME_OUT"}};

void sw_call_back(uint gpio, uint32_t event_mask);

SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_GPIO, cfg_central_switch);
SwitchButtonWithIRQ encoder_clk = SwitchButtonWithIRQ(ENCODER_CLK_GPIO, &sw_call_back, cfg_encoder_clk);

void sw_call_back(uint gpio, uint32_t event_mask)
{
    encoder_clk.irq_enabled(false);
    pr_D3.hi();
    if (gpio == CENTRAL_SWITCH_GPIO)
        printf("IRQ on CENTRAL_SWITCH_GPIO\n");

    if (gpio == ENCODER_CLK_GPIO)
    {
        ControlEvent clk_event = encoder_clk.process_IRQ_event(event_mask);
        if (clk_event != ControlEvent::NOOP)
        {
            pr_D4.pulse_us(1); // actual IRQ received
            printf("ENCODER_CLK_GPIO IRQ event(%s) mask(%d)\n", event_to_string[clk_event].c_str(), event_mask);
        }
        else
            pr_D5.pulse_us(1); // NOOP indicating bounces on clk_event
    }
    pr_D3.lo();
    encoder_clk.irq_enabled(true);
};

int main()
{
    stdio_init_all();

    while (true)
    {
        ControlEvent central_switch_event = central_switch.process_sample_event();
        if (central_switch_event != ControlEvent::NOOP)
            printf("central switch sampled event(%s)\n", event_to_string[central_switch_event].c_str());
    }

    return 0;
}
