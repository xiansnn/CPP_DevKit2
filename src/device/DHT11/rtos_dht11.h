/**
 * @file rtos_dht11.h
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-08-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once
#include "hardware/gpio.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"


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
 * \ingroup sensor
 */
class rtos_DHT11
{
private:
    /// @brief the GPIO connected to DHT11
    uint gpio_in;

    /// @brief the output queue that receives readings
    QueueHandle_t dht_reading_queue;

public:

    /// @brief the function called by a RTOS periodic task 
    void read_from_dht();

    /// @brief the constructor for rtos_DHT11 object
    /// @param gpio_in the gpio number connected the data pin
    /// @param dht_reading_ouput_queue the output queue that receives readings
    rtos_DHT11(uint gpio_in, QueueHandle_t dht_reading_ouput_queue);
};
