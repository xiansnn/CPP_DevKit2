#include "probe.h"
#include "hardware/gpio.h"

Probe::Probe(uint _channel)
{
    /* initialisation of the probe instance
    */
    this->_gpio = Probe::_channel_map[_channel];
    gpio_init(this->_gpio);
    gpio_set_dir(this->_gpio, GPIO_OUT);
}

void Probe::hi()
{
    gpio_put(this->_gpio, 1);
}

void Probe::lo()
{
    gpio_put(this->_gpio, 0);
}

void Probe::pulse_us(uint _duration)
{
    hi();
    busy_wait_us(_duration);
    lo();
}

void Probe::copy(bool _value)
{
    gpio_put(this->_gpio, _value);
}
