/**
 * @file hc_sr04.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief
 * @version 0.1
 * @date 2024-12-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#if !defined(HC_SR04_H)
#define HC_SR04_H

#include "pico/stdlib.h"

/**
 * @brief class for the ultrasonic ranging module HC-SR04
 *
 */
class HC_SR04
{
private:
    uint trig_pin;
    uint echo_pin;
    /// @brief send a trig signal to HC_SR04
    void trig();

public:
    /**
     * @brief Construct a new hc sr04 object
     *
     * @param trig_pin the pin attached to the triggering signal
     * @param echo_pin the pin used to measure round-trip time of ultrasonic pulses
     */
    HC_SR04(uint trig_pin, uint echo_pin);
    /**
     * @brief request a measure from HC_SR04
     *
     * @return float the measured distance
     */
    float get_distance();
};

#endif // HC_SR04_H
