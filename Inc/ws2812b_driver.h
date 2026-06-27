#define F_CPU 16000000UL // для задержек в delay.h

#include <avr/io.h>
#include <util/delay.h>

void init();
void show(uint8_t *data, uint16_t len);