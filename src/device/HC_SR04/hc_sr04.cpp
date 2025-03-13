#include "pico/stdio.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "device/hc_sr04.h"

HC_SR04::HC_SR04(uint trig_pin, uint echo_pin)
{
    this->trig_pin = trig_pin;
    this->echo_pin = echo_pin;
    gpio_init(this->trig_pin);
    gpio_init(this->echo_pin);
    gpio_set_dir(this->trig_pin, GPIO_OUT);
    gpio_set_dir(this->echo_pin, GPIO_IN);
    gpio_pull_up(this->echo_pin);
}

void HC_SR04::trig()
{
    gpio_put(this->trig_pin, 1);
    sleep_us(10);
    gpio_put(this->trig_pin, 0);
}

float HC_SR04::get_distance()
{
    this->trig();
    // wait
    while (gpio_get(this->echo_pin) == 0)
    {
        tight_loop_contents();
    }
    absolute_time_t start = get_absolute_time();
    while (gpio_get(this->echo_pin) == 1)
    {
        if (absolute_time_diff_us(start, get_absolute_time()) > 30000)
            return -1;
    }
    absolute_time_t end = get_absolute_time();
    int64_t travel_time = absolute_time_diff_us(start, end);
    float range = (float)travel_time * 0.017; // 340m/s give 0.0340 cm/us round-trip -> 0.017 cm/us
    return range;
}
