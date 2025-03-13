#include "device/hc_sr04.h"
#include <stdio.h>

#define TRIG_PIN 21
#define ECHO_PIN 26

int main()
{
    stdio_init_all();
    HC_SR04 ranging_device = HC_SR04(TRIG_PIN, ECHO_PIN);
    float range{};

    while (true)
    {
        range = ranging_device.get_distance();
        sleep_ms(500);
        printf("range: %.2f cm\n", range);
    }

    return 0;
}
