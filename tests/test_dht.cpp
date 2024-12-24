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


#ifdef PICO_DEFAULT_LED_PIN
#define LED_PIN PICO_DEFAULT_LED_PIN
#endif

#define CURRENT_DHT_PIN 16 // to be changed to the actual gpio pin

int main()
{
    DHT11 dht = DHT11(CURRENT_DHT_PIN);
    stdio_init_all();
    gpio_init(CURRENT_DHT_PIN);
#ifdef LED_PIN
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
#endif
    gpio_init(18); // yellow LED on Makerfab pico board
    gpio_set_dir(18, GPIO_OUT);
    gpio_put(18, 0);
    gpio_init(19); // green LED on Makerfab pico board
    gpio_set_dir(19, GPIO_OUT);
    gpio_put(19, 0);

    while (1)
    {
        gpio_put(18, 1);
        struct_DHTReading reading;
#ifdef LED_PIN
        gpio_put(LED_PIN, 1);
#endif
        dht.read_from_dht(&reading);
#ifdef LED_PIN
        gpio_put(LED_PIN, 0);
#endif
        if (reading.is_valid)
        {
            float fahrenheit = (reading.temp_celsius * 9 / 5) + 32;
            printf("Humidity = %.1f%%, Temperature = %.1fC (%.1F)\n",
                   reading.humidity, reading.temp_celsius, fahrenheit);
        }
        else
        {
            printf("Checksum error\n");
        }
        gpio_put(18, 0);
        gpio_put(19, 1);
        sleep_ms(2000);
        gpio_put(19, 0);
    }
}