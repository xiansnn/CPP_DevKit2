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
#include <map>
#include <string>

#include "FreeRTOS.h"
#include "task.h"
// #include "queue.h"
#include "event_groups.h"

#include "device/switch_button/switch_button.h"
#include "utilities/probe/probe.h"

#define CENTRAL_SWITCH_GPIO 17
#define ENCODER_CLK_GPIO 21

Probe p0 = Probe(0);
Probe p1 = Probe(1);
Probe p2 = Probe(2);
Probe p3 = Probe(3);
Probe p4 = Probe(4);

struct_ConfigSwitchButton cfg_central_switch{
    .debounce_delay_us = 5000,
    .long_release_delay_us = 1000000,
    .long_push_delay_us = 1000000,
    .time_out_delay_us=4000000};

struct_ConfigSwitchButton cfg_encoder_clk{
    .debounce_delay_us = 1000,
};

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

void irq_call_back(uint gpio, uint32_t event_mask);

SwitchButton central_switch = SwitchButton(CENTRAL_SWITCH_GPIO, cfg_central_switch);
SwitchButtonWithIRQ encoder_clk = SwitchButtonWithIRQ(ENCODER_CLK_GPIO, &irq_call_back, cfg_encoder_clk);

void irq_call_back(uint gpio, uint32_t event_mask)
{
    encoder_clk.irq_enabled(false);
    p3.hi();
    if (gpio == CENTRAL_SWITCH_GPIO)
        printf("IRQ on CENTRAL_SWITCH_GPIO\n");

    if (gpio == ENCODER_CLK_GPIO)
    {
        UIControlEvent clk_event = encoder_clk.process_IRQ_event(event_mask);
        if (clk_event != UIControlEvent::NONE)
        {
            p4.pulse_us(1); // actual IRQ received
            printf("ENCODER_CLK_GPIO IRQ event(%s) mask(%d)\n", event_to_string[clk_event].c_str(), event_mask);
        }
        // else
        //     pr_D5.pulse_us(1); // NONE indicating bounces on clk_event
    }
    p3.lo();
    encoder_clk.irq_enabled(true);
};

/**
 * @brief this test program execises both switch_button classes, with and without IRQ
 * 
 * @return int 
 */
int main()
{
    stdio_init_all();

    while (true)
    {
        UIControlEvent central_switch_event = central_switch.process_sample_event();
        if (central_switch_event != UIControlEvent::NONE)
            printf("central switch sampled event(%s)\n", event_to_string[central_switch_event].c_str());
    }

    return 0;
}
