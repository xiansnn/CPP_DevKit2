/**
 * @file ky040.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "ky040.h"

KY040::KY040(uint central_switch_gpio, 
        uint encoder_clk_gpio, 
        uint encoder_dt_gpio, 
        gpio_irq_callback_t call_back, 
        struct_ConfigSwitchButton sw_conf, 
        struct_ConfigSwitchButton clk_conf  )
        : UIController()
{
        this->central_switch = SwitchButton(central_switch_gpio,sw_conf);
        this->encoder = RotaryEncoder(encoder_clk_gpio,encoder_dt_gpio,call_back,nullptr,clk_conf);
}

KY040::~KY040()
{
}

void KY040::process_encoder_IRQ(uint32_t event_mask)
{
        this->encoder.interrupt_service_routine(event_mask);
}

UIControlEvent KY040::process_central_switch_event()
{
       return this->central_switch.process_sample_event();
}

void KY040::update_UI_control_event_processor(control_event_processor_t event_processor)
{
        this->encoder.update_event_processor(event_processor);
}
