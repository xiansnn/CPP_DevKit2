/**
 * @file rtos_hc_sr04.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2025-08-19
 *
 * @copyright Copyright (c) 2025
 *
 */
#pragma once

#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/// @brief data structure used by ISR to capture, mask, time.
struct struct_HCSR04IRQData
{
    /// @brief the current time at wich the IRQ occurred
    uint32_t current_time_us;
    /// @brief the IRQ mask given by the IRQ harware
    uint32_t event_mask;
};

/**
 * @brief class for the ultrasonic ranging module HC-SR04 compliant with FreeRTOS
 * \ingroup sensor
 */
class rtosHC_SR04
{
private:
    /// @brief the GPIO pin that send trigger signal to the HC-SR04 module
    uint trig_pin;
    /// @brief the GPIO pin where the HC-SR04 return echo signal
    uint echo_pin;
    /// @brief A memory slot reserved to store the irq_event_mask.
    uint32_t echo_irq_mask_config;
    /// @brief the queue from which echo time measurement are received
    QueueHandle_t input_timer_queue;
    /// @brief the queue to which the resulting range is sent
    QueueHandle_t output_range_queue;

    bool measure_completed;

public:

    /// @brief Construct a new rtosHC_SR04 object
    /// @param trig_pin the pin attached to the triggering signal
    /// @param echo_pin the pin used to measure round-trip time of ultrasonic pulses
    /// @param input_timer_queue the input queue that receives data from IRQ
    /// @param output_range_queue the output queue that receives computed range
    /// @param echo_irq_call_back The ISR (interrupt Service Routine) that process IRQ event
    /// @param event_mask_config the rising/falling edge configuratio of the irq
    rtosHC_SR04(uint trig_pin, uint echo_pin,
                QueueHandle_t input_timer_queue, QueueHandle_t output_range_queue,
                gpio_irq_callback_t echo_irq_call_back, uint32_t event_mask_config = GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE);

    /// @brief request a measure from HC_SR04
    void get_distance();
};
