/**
 * @file dht11.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

#include "hardware/gpio.h"



/**
 * @brief the sensor result is given according to this structure:
 * 
 */
struct struct_DHTReading
{
    /// @brief a flag that indicates the reading from DHT11 is valid
    bool is_valid;
    /// @brief the converted values in % of relative humidity
    float humidity;
    /// @brief the converted value in °Celsius
    float temp_celsius;
} ;


/**
 * @brief the Class manages the DHT11 humidity and temperature sensor
 * 
 */
class DHT11
{
private:
    /// @brief the GPIO connected to DHT11
    uint gpio_in;

public:
    /**
     * @brief the member used to read and convert the values send by DHT11
     * 
     * @param result according to struct_DHTReading structure
     */
    void read_from_dht(struct_DHTReading *result);
    /**
     * @brief Construct a new DHT11 object
     * 
     * @param gpio_in_ the GPIO connected to DHT11
     */
    DHT11(uint gpio_in_);
};
