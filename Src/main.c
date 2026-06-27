#include "ws2812b_driver.h"

int main(void)
{
    init();
    uint8_t led[3];
    while (1)
    {
        led[0] = 250; // G
        led[1] = 250;   // R
        led[2] = 250; // B
        show(led, 3);
        _delay_ms(500);
        led[0] = 0;   // G
        led[1] = 0;   // R
        led[2] = 250;   // B
        show(led, 3);
        _delay_ms(500);
    }
}