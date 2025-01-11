/**
 * @file rotary_encoder.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#if !defined(ROTARY_ENCODER_H)
#define ROTARY_ENCODER_H

#include "switch_button.h"

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
 * As a UIController, the rotary encoder is associated with a UIModelObject.
 *
 */
class RotaryEncoder : public SwitchButtonWithIRQ
{
private:
  /// @brief the GPIO pin connected to the Data pin of the rotary encoder
  uint dt_gpio;
  /// @brief the function pointer that will processes the event such as PUSH, RELEASE, INCREMENT, DECREMENT etc.
  control_event_processor_t event_processor;

public:
  /**
   * @brief Construct a new Rotary Encoder object
   *
   * @param encoder_clk_gpio
   * @param encoder_dt_gpio
   * @param call_back
   * @param event_processor
   * @param clk_conf
   */
  RotaryEncoder(uint encoder_clk_gpio,
                uint encoder_dt_gpio,
                gpio_irq_callback_t call_back,
                control_event_processor_t event_processor,
                struct_SwitchButtonConfig clk_conf = {});

  RotaryEncoder();
  /**
   * @brief Destroy the Rotary Encoder object
   *
   */
  ~RotaryEncoder();
  /**
   * @brief the ISR that process the IRQ and send UIControlEvent to the  UIModelObject currentControlledObject.
   *
   * @param irq_event_mask
   */
  void interrupt_service_routine(uint32_t irq_event_mask);

  /**
   * @brief This member is used to update the function pointer to the UIController.
   * 
   * Notice: we can not directly points to the bound member!
   *
   * @param event_processor
   */
  void update_event_processor(control_event_processor_t event_processor);
};

#endif // ROTARY_ENCODER_H
