/**
 * @file msgeq7.h
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
#include <vector>
#include "hardware/adc.h"
#include <array>

/**
 * @brief The MSGEQ7 is an 7-band audio filter.
 *
 * The band frequencies are centered around: 63Hz, 160Hz, 400Hz, 1000Hz, 2500Hz, 6250Hz, 16000Hz.
 * After a pulse on "reset_gpio" pin, a peak detector for each band is multiplexed on the pin "signal_out" at each pulse on pin "strobe_gpio".
 * \ingroup sensor
 */
class MSGEQ7
{
private:
    /// @brief the GPIO pin the send strobe signal to the MSGEQ7 device
    uint strobe_gpio;
    /// @brief the GPIO pin the send reset signal to the MSGEQ7 device
    uint reset_gpio;
    /// @brief the GPIO pin where the amplitude of the band filter is read
    uint signal_out;
    /// @brief the 7-band resulting measures
    std::array<uint16_t, 7> band_results;

public:
    /**
     * @brief Construct a new MSGEQ7 object
     *
     * @param strobe_gpio the gpio connected to strobe
     * @param reset_gpio  the gpio connected to reset
     * @param signal_out the gpio connected to the analog multiplexed peak detector
     */
    MSGEQ7(uint strobe_gpio, uint reset_gpio, uint signal_out);
    /**
     * @brief Destroy the MSGEQ7 object
     *
     */
    ~MSGEQ7();
    /**
     * @brief Get the spectrum object
     *
     * @return std::array<uint16_t,7> a table with 16bit values for each band
     */
    std::array<uint16_t, 7> get_spectrum();
};

