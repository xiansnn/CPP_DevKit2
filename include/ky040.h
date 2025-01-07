/**
 * @file t_KY040.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-06
 *
 * @copyright Copyright (c) 2025
 *
 */

#if !defined(KY040_H)
#define KY040_H

#include "rotary_encoder.h"
#include "ui_core.h"

class KY040 : public UIController
{
private:
    SwitchButton central_switch;
    RotaryEncoder encoder;

public:
    KY040(uint central_switch_gpio,
          uint encoder_clk_gpio,
          uint encoder_dt_gpio,
          gpio_irq_callback_t call_back = nullptr,
          struct_SwitchButtonConfig sw_conf = {},
          struct_SwitchButtonConfig clk_conf = {});
    ~KY040();
    void process_encoder_IRQ(uint32_t event_mask);
    UIControlEvent process_central_switch_event();
    void update_UI_control_event_processor(control_event_processor_t event_processor);
};

#endif // KY040_H
