/**
 * @file switch_button.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#if !defined(SWITCH_BUTTON_H)
#define SWITCH_BUTTON_H

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "control_event.h"



/// @brief the boolean meaning for GPIO_HI
#define GPIO_HI true

/// @brief the boolean meaning for GPIO_LO
#define GPIO_LO false



/**
 * @brief The logical state of the switch
 *
 */
enum class ButtonState
{
    /// @brief The button is inactive
    IDLE,
    /// @brief the switch has been pushed, the button is active
    ACTIVE,
    /// @brief  A long push has been detected, the switch wait to be released
    RELEASE_PENDING,
    /// @brief the switch has beeb released, the button wait for next action. If nothing occurs, a time_out event is returned and the button is inactive
    TIME_OUT_PENDING
};


 /// @brief the default value for debounce_delay_us
#define DEBOUNCE_us 10000             // default to 10ms
/// @brief the default value for LONG_RELEASE_DELAY_us
#define LONG_RELEASE_DELAY_us 1000000 // default to 1s
/// @brief the default value for LONG_PUSH_DELAY_us
#define LONG_PUSH_DELAY_us 1000000    // default to 1s
/// @brief the default value for TIME_OUT_DELAY_us
#define TIME_OUT_DELAY_us 5000000     // default top 5s

/**
 * @brief These are the values used to configure a switch button
 *
 */
struct struct_SwitchButtonConfig
{
    /**
     * @brief The time during which all changes in the switch state is ignored
     *
     */
    uint debounce_delay_us = DEBOUNCE_us;
    /**
     * @brief if the switch is released after long_release_delay_us (in microseconds) a ControlEvent::RELEASED_AFTER_LONG_TIME is returned,
     * else a ControlEvent::RELEASED_AFTER_SHORT_TIME is released.
     */
    uint long_release_delay_us = LONG_RELEASE_DELAY_us;
    /**
     * @brief when a switch is pushed more than long_push_delay_us (in microseconds) a ControlEvent::LONG_PUSH is returned.
     */
    uint long_push_delay_us = LONG_PUSH_DELAY_us;

    /**
     * @brief when a switch is released and not pushed again for time_out_delay_us (in microseconds) a ControlEvent::TIME_OUT is returned.
     *
     */
    uint time_out_delay_us = TIME_OUT_DELAY_us;
    /**
     * @brief if true, this indicates that when the switch is pushed, a logical LO (0) signal is read on GPIO pin.
     *
     */
    bool active_lo = true;
};

/**
 * @brief SwitchButton status is sampled periodically by software.
 *
 * - Switch status is the status of the physical (i.e. mechanical) switch device.
 *
 * - Button status is the logical status of the button (regardless if the switch is wired active Lo or HI).
 *
 * During each period, the status of the button is compared to the previous status and the function member process_sample_event() return an event accordingly.
 *
 * SwitchButton can be associated with UIController if button belongs to a GUI. In such case a new class must be created that inherits from SwitchButton and UIController.
 *
 * \image html button.png "SwitchButton times references"
 */
class SwitchButton
{
private:
protected:
    /*time related members*/
    /// @brief The time during which all changes in the switch state is ignored
    uint debounce_delay_us;

    /// @brief when a button is pushed more than long_push_delay_us (in microseconds) a ControlEvent::LONG_PUSH is returned.
    uint long_push_delay_us;

    /**
     * @brief if the button is released after long_release_delay_us (in microseconds) a ControlEvent::RELEASED_AFTER_LONG_TIME is returned,
     * else a ControlEvent::RELEASED_AFTER_SHORT_TIME is released.
     */
    uint long_release_delay_us;

    /**
     * @brief if the button is released after time_out_delay_us (in microseconds) a ControlEvent::TIME_OUT is returned,
     *
     */
    uint time_out_delay_us;

    /*mechanical switch related members*/
    /// @brief the GPIO that reads the logical state of the switch (pushed or released)
    uint gpio;

    /// @brief his indicates that when the switch is pushed, a logical LO (0) signal is read.
    bool active_lo;

    /// @brief the system time stored on the previous switch state change.
    uint previous_change_time_us;

    /**
     * @brief return the status of the switch.
     *
     * @return true if switch status is read LO (resp. HI) if active_lo is true (resp. false)
     * @return false if switch status is read HI (resp. LO) if active_lo is true (resp. false)
     */
    bool is_switch_pushed();

    /// @brief The previous state read during the previous period.
    bool previous_switch_pushed_state;

    /*logical button related members*/
    /// @brief the logical button status, required to manage the event returned when the switch is pushed or released.
    ButtonState button_status{ButtonState::IDLE};

public:
    /**
     * @brief Construct a new SwitchButton object
     *
     * @param gpio the microcontroller GPIO that read the switch status
     * @param conf the configuration data according struct_SwitchButtonConfig
     */
    SwitchButton(uint gpio, struct_SwitchButtonConfig conf = {});
    /**
     * @brief Destroy the SwitchButton object
     *
     */
    ~SwitchButton();

    /**
     * @brief the periodic routine that process deboucing, push and release of the switch.
     *
     * @return ControlEvent
     */
    ControlEvent process_sample_event();

    /**
     * @brief Get the button status object
     *
     * @return ButtonState
     */
    ButtonState get_button_status();
};

/**
 * @brief SwitchButtonWithIRQ status is processed by an Interrupt Service Routine.
 * It is derived from SwithButton, but debouncing, press and release are processed differently.
 * 
 * NOTICE: LONG_PUSH and TIME_OUT cannot be implemented by processing IRQ.
 *
 * SwitchButtonWithIRQ can be associated with UIController only if button belongs to a UI.
 * 
 * NOTICE: the test program for switch button with IRQ is implmented with the rotary encoder device, wich is a good example of what can be done
 * with and without IRQ
 *
 */
class SwitchButtonWithIRQ : public SwitchButton
{
private:
    /**
     * @brief A memory slot reserved to store the irq_event_mask.
     */
    uint32_t irq_event_mask_config;
    /**
     *@brief return the logical status of the switch. It process rising and falling edges of the interrupt, according to the active_lo status of the switch.
     *
     * @param current_event_mask
     * @return true if switch status is read LO (resp. HI) if active_lo is true (resp. false)
     * @return false if switch status is read HI (resp. LO) if active_lo is true (resp. false)
     */
    bool is_switch_pushed(uint32_t current_event_mask);

protected:
public:
    /**
     * @brief and function used to enable/disable interrupt during Interrupt Service Routine (ISR)
     *
     * @param enabled
     */
    void irq_enabled(bool enabled);
    /**
     * @brief Construct a new SwitchButtonWithIRQ object
     *
     * @param gpio The microcontroller GPIO connected to the switch
     * @param call_back tie ISR (interrupt Service Routine) that process IRQ event
     * @param conf the configuration value of the switch
     * @param event_mask_config the rising/falling edge configuratio of the irq
     */
    SwitchButtonWithIRQ(uint gpio, gpio_irq_callback_t call_back, struct_SwitchButtonConfig conf = {},
                        uint32_t event_mask_config = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);
    /**
     * @brief Destroy the SwitchButtonWithIRQ object
     *
     */
    ~SwitchButtonWithIRQ();
    /**
     * @brief Process IRQ event and return the resulting event.
     *
     * @param current_event_mask
     * @return ControlEvent
     */
    ControlEvent process_IRQ_event(uint32_t current_event_mask);
};

#endif // SWITCH_BUTTON_H
