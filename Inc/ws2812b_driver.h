#define F_CPU 8000000UL // для задержек в delay.h

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init();
void show();
void color_select(uint8_t led_number, uint8_t r, uint8_t g, uint8_t b);
void all_disable();