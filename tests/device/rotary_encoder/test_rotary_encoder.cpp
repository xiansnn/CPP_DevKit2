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
#include <map>
#include <string>

#include "device/rotary_encoder/rotary_encoder.h"
#include "utilities/probe/probe.h"

#define CENTRAL_SWITCH_GPIO 6
#define ENCODER_CLK_GPIO 26
#define ENCODER_DT_GPIO 21

Probe pr_D3 = Probe(3); // encoder_clk_irq_call_back is running
Probe pr_D4 = Probe(4); // clk_event != SwitchButtonEvent::NONE
Probe pr_D5 = Probe(5); // bounces discarded
// channel 6 : encoder DT pin
// channel 7 : encoder clk pin

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

/// @brief It seems that we need to declare and define this ISR at global scope to handle the Interrupt Request atached to the ENCODER_CLK_GPIO
/// @param gpio the ENCODER_CLK_GPIO
/// @param event_mask the event mask returned by the interrupt hardware routine
void encoder_clk_irq_call_back(uint gpio, uint32_t event_mask);

int number_of_event = 0;

/**
 * @brief event_processor is a function that manages what to do 
 * when INCREMENT or DECREMENT event are raised, according to the application specifications.
 * 
 * This function is called inside the irq_call_back triggered by the encoder
 * @param event the control event  (INCREMENT or DECREMENT) generated by the rotary encoder
 */
void manager_process_control_event(UIControlEvent event)
{

    printf("Encoder event(%s) #%d \n", event_to_string[event].c_str(), number_of_event);
    number_of_event++;
};

RotaryEncoder encoder = RotaryEncoder(ENCODER_CLK_GPIO, 
                                        ENCODER_DT_GPIO,
                                        &encoder_clk_irq_call_back, 
                                        &manager_process_control_event, 
                                        cfg_encoder_clk);

void encoder_clk_irq_call_back(uint gpio, uint32_t event_mask)
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
