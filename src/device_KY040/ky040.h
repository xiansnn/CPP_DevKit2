/**
 * @file ky040.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include "device_rotary_encoder/rotary_encoder.h"
#include "sw_ui_core/ui_core.h"

/**
 * @brief This class implement the module device KY040 which is a rotary encoder with a centrl switch push button.
 * 
 * It inherits from UIController, because most of the time it will be used in a UI context. 
 * Otherwise, a separate implementation of RotaryEncoder aand/or SwitchButton will be suficient.
 * 
 */
class KY040 : public UIController
{
private:
    /// @brief the internal part of KY040 that behaves as a simple switch button
    SwitchButton central_switch;
    /// @brief the internal part of the KY040 that behaves as a rotary encoder
    RotaryEncoder encoder;

public:
    /**
     * @brief Construct a new KY040 object
     * 
     * @param central_switch_gpio  The GPIO pin connected to the central switch push button
     * @param encoder_clk_gpio The GPIO pin connected to the clock signal that trig the IRQ on the rotary encoder
     * @param encoder_dt_gpio The GPIO pin connected to the data signal that indicates the direction of the rotary encoder
     * @param call_back The interupt service routine call by the clock signal IRQ
     * @param sw_conf The central switch push button configuration for debounce processing
     * @param clk_conf The clock signal configuration for debounce processing
     */
    KY040(uint central_switch_gpio,
          uint encoder_clk_gpio,
          uint encoder_dt_gpio,
          gpio_irq_callback_t call_back = nullptr,
          struct_ConfigSwitchButton sw_conf = {},
          struct_ConfigSwitchButton clk_conf = {});
    ~KY040();
    /**
     * @brief the call back function used to hide encoder from KY040 user
     * 
     * @param event_mask  the IRQ data returned by the interrupt manager
     */
    void process_encoder_IRQ(uint32_t event_mask);
    /**
     * @brief the function used to mask the central switch button from KY040 user
     * 
     * @return UIControlEvent 
     */
    UIControlEvent process_central_switch_event();
    /**
     * @brief the update control event processor function used to hide encoder from KY040 user
     * 
     * @param event_processor a function pointer compliant with control_event_processor_t
     */
    void update_UI_control_event_processor(control_event_processor_t event_processor);
};

