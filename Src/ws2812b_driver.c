#include "ws2812b_driver.h"

void init()
{
    // НАСТРОЙКА ТАКТИРОВАНИЯ
    CCP = CCP_IOREG_gc;       // выключаем защиту с регистров тактирования на следующие 4 такта
    CLKCTRL.MCLKCTRLA = 0x00; // тактирование 16 МГц внутренним генератором
    CCP = CCP_IOREG_gc;       // выключаем защиту с регистров тактирования на следующие 4 такта
    CLKCTRL.MCLKCTRLB = 0x00; // выключаем дефолтный преддилитель
    while (!(CLKCTRL.MCLKSTATUS & (1 << 4)))
    {
    } // проверка что с тактированием всё ок

    // НАСТРОЙКА ПИНА PA2
    PORTA.DIRSET = 1 << 2; // настраиваем PA3 как выход
    PORTA.PIN2CTRL = 0x00; // выключаем подтяжку на всякий случай
    PORTA.OUTCLR = 1 << 2; // ставим 0 на пине на всякий случай
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
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop"); // 14 clk
    VPORTA.OUT &= ~(1 << 2);
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop"); // 5 clk
}

static inline void send_zero()
{
    VPORTA.OUT |= 1 << 2;
    asm volatile("nop"); // 62,5 нс
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop"); // 6 clk
    VPORTA.OUT &= ~(1 << 2);
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
    asm volatile("nop");
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

void show(uint8_t *data, uint16_t len)
{
    uint8_t sreg = SREG;
    asm volatile("cli");

    while (len--)
    {
        send_byte(*(data++));
    }
    SREG = sreg;

    _delay_us(100);
}