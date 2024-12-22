/**
 * @file test_rotary_encoder.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "rotary_encoder.h"
#include "probe.h"
#include <map>
#include <string>

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21

// Probe pr_D1 = Probe(1); // copy of : encoder_clk.is_button_active()
// Probe pr_D2 = Probe(2); // copy of : encoder_central_sw.is_button_active()
Probe pr_D3 = Probe(3); // sw_call_back is running
Probe pr_D4 = Probe(4); // clk_event != SwitchButtonEvent::NOOP
Probe pr_D5 = Probe(5); // bounces discarded
// channel 0 : central switch pin
// channel 6 : encoder DT pin
// channel 7 : encoder clk pin

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

int number_of_event = 0;

void event_processor(ControlEvent event)
{

    printf("Encoder event(%s) #%d \n", event_to_string[event].c_str(), number_of_event);
    number_of_event++;
};

RotaryEncoder encoder = RotaryEncoder(ENCODER_CLK_GPIO, ENCODER_DT_GPIO,
                                      &sw_call_back, &event_processor, cfg_encoder_clk);

void sw_call_back(uint gpio, uint32_t event_mask)
{
    pr_D3.hi();

    if (gpio == ENCODER_CLK_GPIO)
        encoder.interrupt_service_routine(event_mask);
    pr_D3.lo();
}

;

int main()
{
    stdio_init_all();

    while (true)
    {
        sleep_ms(10);
    }

    return 0;
}
