#include "main.h"
#include "ws2812b_driver.h"

uint8_t sleep_mode_flag = 0;

int main(void)
{
    init();
    while (1)
    {
        uint16_t current_battery = read_battery_charge();

        if (sleep_mode_flag)
        {
            go_to_sleep();
            continue;
        }

        if (current_battery >= 4195) // зарядка завершена
        {
            VPORTA.OUT |= (1 << 6);
            color_select(1, 0, 255, 0);
            color_select(2, 0, 255, 0);
            color_select(3, 0, 255, 0);
            show();
        }
        else if (current_battery >= 3000 && current_battery < 4195) // еще заряжается
        {
            VPORTA.OUT &= ~(1 << 6);
            all_disable();
            color_select(1, 0, 0, 255);
            show();
            _delay_ms(2000);
            if (sleep_mode_flag)
                continue;
            all_disable();
            color_select(2, 0, 0, 255);
            show();
            _delay_ms(2000);
            if (sleep_mode_flag)
                continue;
            all_disable();
            color_select(3, 0, 0, 255);
            show();
            _delay_ms(2000);
            if (sleep_mode_flag)
                continue;
            all_disable();
            _delay_ms(2000);
            if (sleep_mode_flag)
                continue;
        }
        else if (current_battery > 1000 && current_battery < 3000) // всё пизда
        {
            go_to_sleep();
        }
        else // тут просто если батарейки нет
        {
            color_select(1, 0, 255, 0);
            color_select(2, 0, 255, 0);
            color_select(3, 0, 255, 0);
            show();
        }
    }
}

uint16_t read_battery_charge()
{
    ADC0.COMMAND = 0x01;            // запуск преобразования
    while (!(ADC0.INTFLAGS & 0x01)) // ожидаем флаг завершения преобразования
    {
    }
    ADC0.INTFLAGS = 0x01; // сбрасываем флаг

    uint32_t raw_adc = ADC0.RES;

    if (raw_adc == 0)
        return 0;

    uint32_t calc = ((raw_adc * 2500) / 1024UL) * 2; // пересчет в реальные мВ

    if (calc > 5000)
        calc = 5000;

    return (uint16_t)calc;
}

void go_to_sleep()
{
    sleep_mode_flag = 1;
    color_select(1, 255, 0, 0);
    color_select(2, 255, 0, 0);
    color_select(3, 255, 0, 0);
    show();
    _delay_ms(2000);
    all_disable();
    _delay_ms(2000);

    VPORTA.OUT |= (1 << 6); // закрываем полевик

    while (!(PORTB.IN & (1 << 1)))
    {
        _delay_ms(10);
    }
    _delay_ms(100);

    PORTB.INTFLAGS = (1 << 1);

    PORTB.PIN1CTRL = (1 << 3) | 0x05;
    SLPCTRL.CTRLA = (0x2 << 1) | 0x01; // уход в сон Power-Down
    asm volatile("sleep");

    // тут МК просыпается после обработки прерывания по кнопке

    while (!(PORTB.IN & (1 << 1)))
    {
        _delay_ms(10);
    }

    PORTB.INTFLAGS = (1 << 1);
    sleep_mode_flag = 0;
}

// обработчик прерываний по кнопке
ISR(PORTB_PORT_vect)
{
    if (PORTB.INTFLAGS & (1 << 1))
    {
        if (sleep_mode_flag == 0) // если отправляем в сон
        {
            sleep_mode_flag = 1;
        }
        else // если выводим из сна
        {
            sleep_mode_flag = 0;
            PORTB.PIN1CTRL = (1 << 3) | 0x03; // включаем прерывания по падению напряжения на пине
        }

        PORTB.INTFLAGS = (1 << 1);
    }
}