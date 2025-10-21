/**
 * @file rtos_rotary_encoder.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-17
 *
 * @copyright Copyright (c) 2025
 *
 */

#pragma once

#include "device/switch_button/rtos_switch_button.h"

/// @brief this is a function pointer used as rotary encoder constructor paramater
typedef void (*control_event_processor_t)(UIControlEvent event);

/**
 * @brief The RotaryEncoder is a device used to control a value the can be incremented/decremented by the user.
 * It can be associated with UIControlledIncrementalValue.
 *
 * A rotary encoder has two signals: one can be considered as a clock (clk) , the other as the data (dt).
 *
 * Clock signal is produced by rotating the encoder. Each time a falling edge is generated on the clock, the value on dt gives the direction of the rotation.
 *
 * Due to the short time between clk and dt, we cannot usually sample the signals. IRQ is more appropriate.
 *
 * As a UIController, the rotary encoder is associated with a Model.
 * \ingroup control
 */
class rtos_RotaryEncoder : public rtos_SwitchButton
{
private:
  /// @brief the GPIO pin connected to the Data pin of the rotary encoder
  uint dt_gpio;
  // /// @brief the function pointer that will processes the event such as PUSH, RELEASE, INCREMENT, DECREMENT etc.
  // control_event_processor_t event_processor;

public:

  /// @brief  Construct a new Rotary Encoder object compliant with FreeRTOS
  /// @param encoder_clk_gpio  Construct a new Rotary Encoder object
  /// @param encoder_dt_gpio  Construct a new Rotary Encoder object
  /// @param call_back the IRQ callbaak
  /// @param in_switch_button_queue the input queue receiveing IRQ data
  /// @param out_control_event_queue the output queue receiving control event
  /// @param conf the clock configutration (as a debounced switch)
  /// @param event_mask_config the IRQ mask
  rtos_RotaryEncoder(uint encoder_clk_gpio,
                    uint encoder_dt_gpio,
                    gpio_irq_callback_t call_back, QueueHandle_t in_switch_button_queue, QueueHandle_t out_control_event_queue,
                    struct_rtosConfigSwitchButton conf = {}, uint32_t event_mask_config = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);

  /**
   * @brief Destroy the Rotary Encoder object
   *
   */
  ~rtos_RotaryEncoder();

  void rtos_process_IRQ_event() override;
 
};
