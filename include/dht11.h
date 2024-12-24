/**
 * @file dht11.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef DHT11_H
#define DHT11_H

#include "hardware/gpio.h"
#include "pico/stdlib.h"



/**
 * @brief the sensor result is given according to this structure:
 * 
 */
typedef struct
{
    /// @brief a flag that indicates the reading from DHT11 is valid
    bool is_valid;
    /// @brief the converted values in % of relative humidity
    float humidity;
    /// @brief the converted value in °Celsius
    float temp_celsius;
} dht_reading_t;


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
     * @param result according to dht_reading_t structure
     */
    void read_from_dht(dht_reading_t *result);
    /**
     * @brief Construct a new DHT11 object
     * 
     * @param gpio_in_ the GPIO connected to DHT11
     */
    DHT11(uint gpio_in_);
};

#endif // DHT11_H