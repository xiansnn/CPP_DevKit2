#include "msgeq7.h"

MSGEQ7::MSGEQ7(uint strobe_gpio, uint reset_gpio, uint signal_out)
{
    this->reset_gpio = reset_gpio;
    this->strobe_gpio = strobe_gpio;
    this->signal_out = signal_out;
    adc_init();
    adc_gpio_init(signal_out);
    adc_select_input(signal_out - 26);
    gpio_init(strobe_gpio);
    gpio_set_dir(strobe_gpio, GPIO_OUT);
    gpio_init(reset_gpio);
    gpio_set_dir(reset_gpio, GPIO_OUT);
    gpio_put(reset_gpio, 0);
    gpio_put(strobe_gpio, 0);
}

MSGEQ7::~MSGEQ7()
{
}

std::array<uint16_t, 7> MSGEQ7::get_spectrum()
{
    // pulse reset
    gpio_put(reset_gpio, 1);
    sleep_us(5);
    gpio_put(reset_gpio, 0);
    sleep_us(70);

    // series of strobe pulses
    for (size_t i = 0; i < 7; i++)
    {
        gpio_put(strobe_gpio, 1);
        sleep_us(20);
        gpio_put(strobe_gpio, 0);
        sleep_us(60);
        band_results[i] = adc_read();
    }
    return band_results;
}
