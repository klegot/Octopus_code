#include "ws2812b_driver.h"

#define LEDS_AMOUNT 3

static uint8_t data[3 * LEDS_AMOUNT];

void init()
{
    // НАСТРОЙКА ТАКТИРОВАНИЯ
    CCP = CCP_IOREG_gc;       // выключаем защиту с регистров тактирования на следующие 4 такта
    CLKCTRL.MCLKCTRLA = 0x00; // тактирование 16 МГц внутренним генератором
    CCP = CCP_IOREG_gc;       // выключаем защиту с регистров тактирования на следующие 4 такта
    CLKCTRL.MCLKCTRLB = 0x01; // выключаем дефолтный преддилитель
    while (!(CLKCTRL.MCLKSTATUS & (1 << 4)))
    {
    } // проверка что с тактированием всё ок

    // НАСТРОЙКА ПИНА PA2
    PORTA.DIRSET = 1 << 2; // настраиваем PA3 как выход
    PORTA.PIN2CTRL = 0x00; // выключаем подтяжку на всякий случай
    PORTA.OUTCLR = 1 << 2; // ставим 0 на пине на всякий случай

    // НАСТРОЙКА АЦП
    ADC0.CTRLA = 0; // включаем 10-битное разрешение АЦП
    ADC0.CTRLC = ADC_PRESC_DIV128_gc | ADC_REFSEL_INTREF_gc;
    ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc; // канал АЦП на пине PA7
    PORTA.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
    VREF.CTRLA = VREF_ADC0REFSEL_2V5_gc;
    ADC0.CTRLA |= 0x01; // включаем АЦП

    // НАСТРОЙКА ПИНА PA6
    PORTA.DIRSET = 1 << 6; // настраиваем PA3 как выход
    PORTA.PIN2CTRL = 0x00; // выключаем подтяжку на всякий случай
    PORTA.OUTSET = 1 << 6; // ставим 1 на пине на всякий случай

    // НАСТРОЙКА ПИНА PB1
    PORTB.DIRCLR = 1 << 1; // настраиваем PB1 как вход
    PORTB.PIN1CTRL = (1 << 3) | 0x02; // включаем подтяжку и прерывания на падающем фронте

    asm volatile("sei"); // глобальное включение прерываний
}

static inline void send_one()
{
    VPORTA.OUT |= 1 << 2;
    asm volatile("nop"); // 62,5 нс
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop"); // 14 clk
    VPORTA.OUT &= ~(1 << 2);
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop"); // 5 clk
}

static inline void send_zero()
{
    VPORTA.OUT |= 1 << 2;
    asm volatile("nop"); // 62,5 нс
    asm volatile("nop");
    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop"); // 6 clk
    VPORTA.OUT &= ~(1 << 2);
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
//    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop"); // 13 clk
}

static inline void send_byte(uint8_t byte)
{
    if (byte & 0x80)
        send_one();
    else
        send_zero();
    if (byte & 0x40)
        send_one();
    else
        send_zero();
    if (byte & 0x20)
        send_one();
    else
        send_zero();
    if (byte & 0x10)
        send_one();
    else
        send_zero();
    if (byte & 0x08)
        send_one();
    else
        send_zero();
    if (byte & 0x04)
        send_one();
    else
        send_zero();
    if (byte & 0x02)
        send_one();
    else
        send_zero();
    if (byte & 0x01)
        send_one();
    else
        send_zero();
}

void color_select(uint8_t led_number, uint8_t r, uint8_t g, uint8_t b)
{
    data[(led_number - 1) * 3] = g;
    data[(led_number - 1) * 3 + 1] = r;
    data[(led_number - 1) * 3 + 2] = b;
}

void all_disable()
{
    for (uint8_t i = 0; i < 3 * LEDS_AMOUNT; i++)
    {
        data[i] = 0;
    }
    show();
}

void show()
{
    uint8_t sreg = SREG;
    asm volatile("cli");
    uint8_t len = 3 * LEDS_AMOUNT;
    uint8_t *ptr = data;
    while (len--)
    {
        send_byte(*(ptr++));
    }
    SREG = sreg;

    _delay_us(100);
}