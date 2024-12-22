#if !defined(ROTARY_ENCODER_H)
#define ROTARY_ENCODER_H

#include <pico/stdio.h>
#include "switch_button.h"

typedef void (*control_event_processor_t)(ControlEvent event);

/**
 * @brief The rotary encoder is a device used to control a value the can be incremented/decremented by the user.
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
 * The test program for rotary encoder is the one developed for the Switch button with IRQ
 * 
 */
class RotaryEncoder : public SwitchButtonWithIRQ//, public UIController
{
private:
    uint dt_gpio;
    control_event_processor_t event_processor;

public:
    /**
     * @brief Construct a new Rotary Encoder object
     * 
     * @param encoder_clk_gpio  The GPIO that produces IRQ
     * @param encoder_dt_gpio  The GPIO that read the rotation direction
     * @param call_back  The Interrupt Service Routine call by IRQ
     * @param event_processor  The pointer to the function in charge of precessing the ControlEvent INCREMENT or DECREMENT
     * @param clk_conf  the configuration of the clock, view as a switch (with debouncing issue)
     */
    RotaryEncoder(uint encoder_clk_gpio, uint encoder_dt_gpio,
                gpio_irq_callback_t call_back, 
                control_event_processor_t event_processor,
                struct_SwitchButtonConfig clk_conf = {});
    /**
     * @brief Destroy the Rotary Encoder object
     * 
     */
    ~RotaryEncoder();
    /**
     * @brief the ISR that process the IRQ and send ControlEvent to the  UIModelObject currentControlledObject.
     * 
     * @param irq_event_mask 
     */
    void interrupt_service_routine(uint32_t irq_event_mask);
  };

#endif // ROTARY_ENCODER_H
