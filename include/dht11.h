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
    bool is_valid;
    float humidity;
    float temp_celsius;
} dht_reading_t;


class DHT11
{
private:
    uint gpio_in;

public:
    void read_from_dht(dht_reading_t *result);
    DHT11(uint gpio_in_);
};

#endif // DHT11_H