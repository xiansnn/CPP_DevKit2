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

#include "device/DHT11/include/dht11.h"
#include <stdio.h>
#include <math.h>
#include "utilities/probe/include/probe.h"

#define CURRENT_DHT_PIN 16 // to be changed to the actual gpio pin

Probe pr_D5 = Probe(5);

int main()
{
    stdio_init_all();
    DHT11 dht = DHT11(CURRENT_DHT_PIN);
    struct_DHTReading dht_reading;

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN,GPIO_OUT);

    while (true)
    {
        pr_D5.hi();
        gpio_put(PICO_DEFAULT_LED_PIN,1);
        dht.read_from_dht(&dht_reading);
        if (dht_reading.is_valid)
        {
            float fahrenheit = (dht_reading.temp_celsius * 9 / 5) + 32;
            printf("Humidity = %.1f%%, Temperature = %.1f°C (%.1F°F)\n",
                   dht_reading.humidity, dht_reading.temp_celsius, fahrenheit);
        }
        else
            printf("Checksum error\n");
        pr_D5.lo();
        gpio_put(PICO_DEFAULT_LED_PIN,0);
        sleep_ms(2000);
    }
}