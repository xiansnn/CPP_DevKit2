
#include <stdio.h>
#include <math.h>

#include "pico/time.h"

#include "device/dht11.h"

#define MAX_TIMING_MEASURE 80

DHT11::DHT11(uint gpio_in_)
{
    this->gpio_in = gpio_in_;
    gpio_init(gpio_in);
}


void DHT11::read_from_dht(struct_DHTReading *result)
{
    int data[5] = {0, 0, 0, 0, 0};
    int64_t measures[MAX_TIMING_MEASURE] = {0};
    uint j = 0;
    gpio_set_dir(this->gpio_in, GPIO_OUT);
    gpio_put(this->gpio_in, 0); // assert measure request for 20ms
    sleep_ms(20);
    gpio_set_dir(this->gpio_in, GPIO_IN); // set Hi and wait for response
    sleep_us(20);                         // wait for sensor to get access to the shared wire

    // skip Start bit 80usLO + 80usHI
    while (gpio_get(this->gpio_in) == 0)
        tight_loop_contents(); // sleep_us(1) or do nothing

    while (gpio_get(this->gpio_in) == 1)
        tight_loop_contents(); // sleep_us(1) or do nothing

    // start data capture.  40 LO-levels + 40 HI-levels  give 80 measures as 5  8-bit words data.
    uint last = gpio_get(this->gpio_in); // first bit acquisition
    absolute_time_t last_measure = get_absolute_time();
    for (uint i = 0; i < MAX_TIMING_MEASURE; i++)
    {
        uint count = 0;
        while (gpio_get(this->gpio_in) == last)
        {
            count++;
            sleep_us(1);
            if (count == 255)
                break;
        }
        last = gpio_get(this->gpio_in);
        absolute_time_t current_time = get_absolute_time();
        int64_t diff = absolute_time_diff_us(last_measure, current_time);
        last_measure = current_time;
        measures[i] = diff; // measure all time intervals
        if (count == 255)
            break;

        // convert measures to binary data
        if (i % 2 == 1)
        {                      // only HI level is significant
            data[j / 8] <<= 1; //
            if (measures[i] > 50)
                data[j / 8] |= 1; // measure < 50us (typically 25us) means "0", measure > 50us (typically 70us) means "1"
            j++;
        }
    }

    // convert data to temperature and humidity reading
    if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
    { // check if checksum is correct
        result->is_valid = true;
        // result->humidity = (float)((data[0] << 8) + data[1]) / 10;
        result->humidity = data[0] + 0.1*data[1];
        if (result->humidity > 100)
        {
            result->humidity = data[0];
        }
        result->temp_celsius = data[2] + 0.1*data[3] ;
        if (result->temp_celsius > 125)
            result->temp_celsius = data[2];

        if (data[2] & 0x80)
            result->temp_celsius = -result->temp_celsius;
    }
    else
        result->is_valid = false;
}
