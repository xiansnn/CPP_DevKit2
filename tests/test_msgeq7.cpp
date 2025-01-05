#include "msgeq7.h"
#include <array>
#include "probe.h"
Probe pr_D4 = Probe(4);

#define MSGEQ7_STROBE 19
#define MSGEQ7_RESET 18
#define MSGEQ7_OUT 27

int main()
{
    stdio_init_all();
    MSGEQ7 spectrum_analyser = MSGEQ7(MSGEQ7_STROBE, MSGEQ7_RESET, MSGEQ7_OUT);
    std::array<uint16_t, 7> spectrum;

    while (true)
    {
        pr_D4.hi();
        spectrum = spectrum_analyser.get_spectrum();
        pr_D4.lo();
        sleep_ms(2);
    }
    return 0;
}
