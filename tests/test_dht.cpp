/**
 * @file test_dht.cpp
 * @author xiansnn (xiansnn@hotmail.com)
 * @brief a test file showing usage of DHT11 device class
 * updated 2023-03-31 : xiansnn, inspired from Adafruit DHT driver
 * @version 0.1
 * @date 2023-04-25
 * @copyright Copyright (c) 2023, Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <math.h>

#include "dht11.h"

#define CURRENT_DHT_PIN 16 // to be changed to the actual gpio pin

int main()
{
    stdio_init_all();
    DHT11 dht = DHT11(CURRENT_DHT_PIN);
    struct_DHTReading reading;

    while (true)
    {
        gpio_put(18, 1);
        dht.read_from_dht(&reading);
        if (reading.is_valid)
        {
            float fahrenheit = (reading.temp_celsius * 9 / 5) + 32;
            printf("Humidity = %.1f%%, Temperature = %.1fC (%.1F)\n",
                   reading.humidity, reading.temp_celsius, fahrenheit);
        }
        else
            printf("Checksum error\n");
        sleep_ms(2000);
    }
}