/**
 * @file probe.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef PROBE_H
#define PROBE_H

#include "pico/stdlib.h"

/**
 * @brief   The Probe provides a convenient class that can be used to observe behavior of programs.
 * It may be used with a logical analyser. The one used here has eight channels, numbered from 0 to 7.
 *
 */
class Probe
{
private:
    /**
     * @brief this is the GPIO output on wich signals will be generated. The numbers are those of Raspberry Pi Pico.
     *
     */
    uint _gpio;
    /**
     * @brief this is the mapping of GPIO number to the logic analyser channel.
     * The choice of GPIO number is arbitrar. It will depends on free outputs on your specific hardware implementation.
     *
     */
    constexpr static int _channel_map[8] = {6, 7, 8, 9, 16, 17, 21, 26};

public:
    /**
     * @brief Construct a new Probe object.
     * 
     * The Probe provides a convenient class that can be used to observe behavior of programs.
     * It may be used with a logical analyser. The one used here has eight channel, numbered from 0 to 7.
     *
     * @param _channel the channel number as it appears on the logic analyser display
     */
    Probe(uint _channel);
    /**
     * @brief Generates a HI logical level on the GPIO
     *
     */
    void hi();
    /**
     * @brief Generates a LO logical level on the GPIO
     *
     */
    void lo();
    /**
     * @brief Generates a HI level pulse on the GPIO during _duration µs.
     *
     * @param _duration
     */
    void pulse_us(uint _duration);
    /**
     * @brief Used to copy the boolean _value on th GPIO.
     *
     * @param _value
     */
    void copy(bool _value);
};

#endif