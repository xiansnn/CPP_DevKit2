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