#define F_CPU 3333333UL
#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN_bm         (1 << 2) // Пин PA2

// Высокоточный вывод через стандартные базовые команды sbi/cbi
// 1 такт процессора = ровно 300 наносекунд!
static inline void send_bit_vport(uint8_t bit) {
    if (bit) {
        // Бит "1": Нам нужно ~800-900нс HIGH и ~300-450нс LOW.
        // 1. sbi включает HIGH (1 такт = 300 нс)
        // 2. rjmp (2 такта = 600 нс) просто ждет. Итого HIGH = 900 нс.
        // 3. cbi выключает в LOW.
        __asm__ __volatile__ (
            "sbi 0x01, 2 \n\t"     // Виртуальный порт VPORTA.OUT (0x01), пин 2 в HIGH (300 нс)
            "rjmp .+0 \n\t"        // Пауза (2 такта = 600 нс)
            "cbi 0x01, 2 \n\t"     // В LOW (300 нс)
        );
    } else {
        // Бит "0": Нам нужно ~300-400нс HIGH и ~800-900нс LOW.
        // 1. sbi включает HIGH (1 такт = 300 нс).
        // 2. Мгновенно в следующем такте cbi выключает в LOW. Итого HIGH = 300 нс (Идеальный ноль).
        __asm__ __volatile__ (
            "sbi 0x01, 2 \n\t"     // В HIGH (1 такт = 300 нс)
            "cbi 0x01, 2 \n\t"     // В LOW (1 такт = 300 нс)
            "rjmp .+0 \n\t"        // Компенсируем паузу LOW между битами (2 такта)
        );
    }
}

void send_pixel_direct(uint8_t r, uint8_t g, uint8_t b) {
    uint8_t sreg_save = SREG;
    __asm__ __volatile__("cli" ::: "memory"); // Отключаем прерывания

    // Передаем строго Green, Red, Blue (от MSB к LSB)
    for (int8_t i = 7; i >= 0; i--) { send_bit_vport(g & (1 << i)); }
    for (int8_t i = 7; i >= 0; i--) { send_bit_vport(r & (1 << i)); }
    for (int8_t i = 7; i >= 0; i--) { send_bit_vport(b & (1 << i)); }

    SREG = sreg_save; // Включаем прерывания обратно
    _delay_us(85);    // Сигнал RESET (>50 мкс)
}

int main(void) {
    // Настройка пина PA2 на выход через прямые адреса регистров ATtiny414
    *(volatile uint8_t *)(0x0401) = LED_PIN_bm; // PORTA.DIRSET
    *(volatile uint8_t *)(0x0406) = LED_PIN_bm; // PORTA.OUTCLR
    *(volatile uint8_t *)(0x0412) = 0x00;        // PORTA.PIN2CTRL (выключаем подтяжку)
    _delay_ms(50);

    while (1) {
        // 1. Включаем КРАСНЫЙ цвет на 1 секунду
        send_pixel_direct(120, 0, 0);
        _delay_ms(1000);

        // 2. Полностью ГАСИМ светодиод на 1 секунду
        send_pixel_direct(0, 0, 0);
        _delay_ms(1000);

        // 3. Включаем ЗЕЛЕНЫЙ цвет на 1 секунду
        send_pixel_direct(0, 120, 0);
        _delay_ms(1000);

        // 4. Полностью ГАСИМ светодиод на 1 секунду
        send_pixel_direct(0, 0, 0);
        _delay_ms(1000);
    }
}
