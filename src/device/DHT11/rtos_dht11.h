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
class rtosDHT11
{
private:
    /// @brief the GPIO connected to DHT11
    uint gpio_in;

    QueueHandle_t dht_reading_queue;

public:
    /**
     * @brief the member used to read and convert the values send by DHT11
     * 
     * @param result according to struct_DHTReading structure
     */
    void read_from_dht();

    rtosDHT11(uint gpio_in, QueueHandle_t dht_reading_ouput_queue);
};
