/**
 * @file ui_control_event.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-01-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

/**
 * @brief The list of predefined events that a button, or more generally an User Interface Controller, can return, leaving to the application the responsibility
 * to act as required by its specification.
 */
enum class UIControlEvent
{

    /// @brief null event, no operation expected.
    NONE,
    /// @brief event triggered when a button is pushed
    PUSH,
    /// @brief event triggered when a button is double-pushed
    /// \todo  Not implemented. To find a way to do "DOUBLE_PUSH"
    DOUBLE_PUSH,
    /// @brief event triggered when a button is held more than a configurable duration.
    LONG_PUSH,
    /// @brief event triggered when a button is released after a configurable duration.
    RELEASED_AFTER_LONG_TIME,
    /// @brief event triggered when a button is released before a configurable duration.
    RELEASED_AFTER_SHORT_TIME,
    /// @brief event that signals the user trig an increment order.
    INCREMENT,
    /// @brief event that signals the user trig an decrement order.
    DECREMENT,
    /// @brief event that signals nothing happens after a configurable period of time.
    TIME_OUT
};

/**
 * @brief this is the structure used to transmit control event through a RTOS queue
 *
 */
struct struct_ControlEventData
{
    /// @brief the control event computed by the IRQ processing task
    UIControlEvent event;
    /// @brief the gpio that receive the IRQ
    int gpio_number;
};

/// @brief The list of status that a Model can have when it has to be controlled by UI device.
/// (0) IS_IDLE
/// (1) IS_WAITING
/// (2) HAS_FOCUS
/// (3) IS_ACTIVE
enum class ControlledObjectStatus
{
    /// @brief The object is in idle state, waiting to be activated.
    IS_IDLE,
    /// @brief the object has done its job and is waiting for the next activation
    ///@note obsolete when using FreeRTOS 
    IS_WAITING,

    /// @brief The widget or object manager is pointing to this model
    HAS_FOCUS,

    /// @brief The user has selected (clicked) on this model. ControlEvent are then passed to this model in order to be processed.
    IS_ACTIVE
};

/// @brief The list of reason of manager time out report.
/// (0) NO_TIME_OUT
/// (1) MANAGER_INACTIVE
/// (3) MANAGED_OBJECT_INACTIVE
enum class ControlledObjectStatusTimeOutReason
{
    /// @brief no time out
    NO_TIME_OUT,
    /// @brief The object is inactive, nothing to do.
    MANAGER_INACTIVE,
    /// @brief The widget or object manager is pointing to this model
    MANAGED_OBJECT_INACTIVE
};
