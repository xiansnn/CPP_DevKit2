/**
 * @file hc_sr04.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include "pico/stdlib.h"

/**
 * @brief class for the ultrasonic ranging module HC-SR04
 * \ingroup sensor
 */
class HCSR04
{
private:
    /// @brief the GPIO pin that send trigger signal to the HC-SR04 module
    uint trig_pin;
    /// @brief the GPIO pin where the HC-SR04 return echo signal
    uint echo_pin;
    /// @brief send a trig signal to HCSR04
    void trig();

public:
    /**
     * @brief Construct a new hc sr04 object
     *
     * @param trig_pin the pin attached to the triggering signal
     * @param echo_pin the pin used to measure round-trip time of ultrasonic pulses
     */
    HCSR04(uint trig_pin, uint echo_pin);
    /**
     * @brief request a measure from HCSR04
     *
     * @return the measured distance in float[cm]. Max = 400cm. If no response, return = -1.
     */
    float get_distance();
};

