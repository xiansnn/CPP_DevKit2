#pragma once

#include "t_rtos_ctrl_focus_config.h"
#include "t_rtos_ctrl_focus_main_model_classes.h"

extern rtos_SwitchButton central_switch;
// extern my_PositionController position_controller;
extern rtos_RotaryEncoder encoder;

extern struct_rtosConfigSwitchButton cfg_central_switch;
extern struct_rtosConfigSwitchButton cfg_encoder_clk;

void ky040_encoder_irq_call_back(uint gpio, uint32_t event_mask);
void central_switch_process_irq_event_task(void *);
void encoder_process_irq_event_task(void *);