/**
 * @file rtos_switch_button.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-14
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>
#include "sw/ui_core/ui_control_event.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/// @brief the boolean meaning for GPIO_HI
#define GPIO_HI true

/// @brief the boolean meaning for GPIO_LO
#define GPIO_LO false

/**
 * @brief The logical state of the switch
 * (0) IDLE
 * (1) ACTIVE
 * (2) RELEASE_PENDING
 * (3) TIME_OUT_PENDING
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
/**
 * @brief this is the structure used to send irq data though the queue
 *
 */
struct struct_IRQData
{
    /// @brief the current time at wich the IRQ occurred
    uint32_t current_time_us;
    /// @brief the IRQ mask given by the IRQ harware
    uint32_t event_mask;
};

/**
 * @brief this is the structure used to transmit control event through the queue
 *
 */
struct struct_ControlEventData
{
    /// @brief the gpio that receive the IRQ
    int gpio_number;
    /// @brief the control event computed by the IRQ processing task
    UIControlEvent event;
};

/// @brief the default value for debounce_delay_us
#define DEBOUNCE_us 10000 // default to 10ms
/// @brief the default value for LONG_RELEASE_DELAY_us
#define LONG_RELEASE_DELAY_us 1000000 // default to 1s
/// @brief the default value for LONG_PUSH_DELAY_us
#define LONG_PUSH_DELAY_ms 1000 // default to 1s
/// @brief the default value for TIME_OUT_DELAY_us
#define TIME_OUT_DELAY_ms 5000 // default 5s/

/**
 * @brief These are the values used to configure a switch button
 * delays related to action of the button (and generate IRQ) are expressed in microsecond, delays related to an absence of action on the button are expressed in milliseconds.
 * (0) debounce_delay_us
 * (1) long_release_delay_us
 * (2) long_push_delay_ms
 * (3) time_out_delay_ms
 * (4) active_lo
 *
 */
struct struct_rtosConfigSwitchButton
{
    /**
     * @brief The time during which all changes in the switch state is ignored
     *
     */
    uint debounce_delay_us = DEBOUNCE_us;
    /**
     * @brief if the switch is released after long_release_delay_us (in microseconds) a UIControlEvent::RELEASED_AFTER_LONG_TIME is sent,
     * else a UIControlEvent::RELEASED_AFTER_SHORT_TIME is sent.
     */
    uint long_release_delay_us = LONG_RELEASE_DELAY_us;
    /**
     * @brief when a switch is maintained pushed more than long_push_delay_ms (in milliseconds) a UIControlEvent::LONG_PUSH is sent.
     */
    uint long_push_delay_ms = LONG_PUSH_DELAY_ms;

    /**
     * @brief when a switch is released and not pushed again for time_out_delay_ms (in milliseconds) a UIControlEvent::TIME_OUT is sent.
     *
     */
    uint time_out_delay_ms = TIME_OUT_DELAY_ms;
    /**
     * @brief if true, this indicates that when the switch is pushed, a logical LO (0) signal is read on GPIO pin.
     *
     */
    bool active_lo = true;
};

/**
 * @brief rtosSwitchButton is processed by an Interrupt Service Routine (ISR) under control of FreeRTOS routines.
 *
 * - Switch status is the status of the physical (i.e. mechanical) switch device.
 *
 * - Button status is the logical status of the button (regardless if the switch is wired active Lo or HI).
 *
 * During each ISR, the status of the button is compared to the previous status and the function member rtos_process_IRQ_event() sends
 *  an event accordingly toward the control_event output queue.
 *
 * SwitchButton can be associated with UIController if button belongs to a GUI. In such case a new class must be created that inherits from rtosSwitchButton and UIController.
 * \ingroup control
 * \image html button_long_release.svg "SwitchButton times references for long release"
 *
 * NOTICE: the test program for rtos switch button is implemented with the rotary encoder device, which is a good example of what can be done
 * with IRQ
 * \ingroup control
 */

class rtosSwitchButton
{
private:
    /**
     * @brief A memory slot reserved to store the irq_event_mask.
     */
    uint32_t irq_event_mask_config;

protected:
    /**
     * @brief return the logical status of the switch. It process rising and falling edges of the interrupt, according to the active_lo status of the switch.
     *
     * @param current_event_mask
     * @return true if switch status is read LO (resp. HI) if active_lo is true (resp. false)
     * @return false if switch status is read HI (resp. LO) if active_lo is true (resp. false)
     */
    bool is_switch_pushed(uint32_t current_event_mask);

    /*time related members*/
    /// @brief The time during which all changes in the switch state is ignored
    uint debounce_delay_us;

    /// @brief when a button is pushed more than long_push_delay_ms (in milliseconds) a UIControlEvent::LONG_PUSH is returned.
    uint long_push_delay_ms;

    /**
     * @brief if the button is released after long_release_delay_us (in microseconds) a UIControlEvent::RELEASED_AFTER_LONG_TIME is returned,
     * else a UIControlEvent::RELEASED_AFTER_SHORT_TIME is released.
     */
    uint long_release_delay_us;

    /**
     * @brief if the button is released after time_out_delay_ms (in milliseconds) a UIControlEvent::TIME_OUT is returned,
     *
     */
    uint time_out_delay_ms;

    /*mechanical switch related members*/
    /// @brief the GPIO that reads the logical state of the switch (pushed or released)
    uint gpio;

    /// @brief his indicates that when the switch is pushed, a logical LO (0) signal is read.
    bool active_lo;

    /// @brief the system time stored on the previous switch state change.
    uint previous_change_time_us;

    /// @brief The previous state read during the previous period.
    bool previous_switch_pushed_state;

    /*logical button related members*/
    /// @brief the logical button status, required to manage the event returned when the switch is pushed or released.
    ButtonState button_status{ButtonState::IDLE};

    /// @brief the queue from which IRQ data are received
    QueueHandle_t switch_button_queue;
    /// @brief the queue to which the resulting control event is sent
    QueueHandle_t control_event_queue;

public:
    /**
     * @brief Construct a new rtos Switch Button object
     *
     * @param gpio The microcontroller GPIO connected to the switch
     * @param call_back The ISR (interrupt Service Routine) that process IRQ event
     * @param in_switch_button_queue the input queue that receives data from IRQ
     * @param out_control_event_queue the output queue that receives computed control event
     * @param conf the configuration value of the switch
     * @param event_mask_config the rising/falling edge configuratio of the irq
     */
    rtosSwitchButton(uint gpio, gpio_irq_callback_t call_back, QueueHandle_t in_switch_button_queue, QueueHandle_t out_control_event_queue,
                     struct_rtosConfigSwitchButton conf = {}, uint32_t event_mask_config = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);

    /**
     * @brief Destroy the SwitchButton object
     *
     */
    ~rtosSwitchButton();

    /**
     * @brief Process IRQ event and sent the resulting event to the event queue
     *
     */
    virtual void rtos_process_IRQ_event() ;
};
