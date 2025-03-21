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

