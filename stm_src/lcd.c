#include "lcd.h"

uint8_t battery_levels[5][8] = {
    { 0b01110, 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111, 0b00000 },
    { 0b01110, 0b11111, 0b10001, 0b10001, 0b10001, 0b11111, 0b11111, 0b00000 },
    { 0b01110, 0b11111, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111, 0b00000 },
    { 0b01110, 0b11111, 0b10001, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000 },
    { 0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000 }
};

void delay_ms(int ms) {
    for (volatile int i = 0; i < ms * 8000; i++);
}

void lcd_pulse_enable() {
    GPIOA->BSRR = (1 << 1);
    delay_ms(1);
    GPIOA->BSRR = (1 << (1 + 16));
    delay_ms(1);
}

void lcd_send_nibble(uint8_t nibble) {
    GPIOA->BSRR = ((nibble & 0x01) ? (1 << 2) : (1 << (2 + 16)));
    GPIOA->BSRR = ((nibble & 0x02) ? (1 << 3) : (1 << (3 + 16)));
    GPIOA->BSRR = ((nibble & 0x04) ? (1 << 4) : (1 << (4 + 16)));
    GPIOA->BSRR = ((nibble & 0x08) ? (1 << 5) : (1 << (5 + 16)));
    lcd_pulse_enable();
}

void lcd_command(uint8_t cmd) {
    GPIOA->BSRR = (1 << (0 + 16));
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0F);
    delay_ms(2);
}

void lcd_data(uint8_t data) {
    GPIOA->BSRR = (1 << 0);
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
    delay_ms(2);
}

void lcd_init() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~((0xF << (0 * 4)) | (0xF << (1 * 4)) | (0xF << (2 * 4)) |
                    (0xF << (3 * 4)) | (0xF << (4 * 4)) | (0xF << (5 * 4)));
    GPIOA->CRL |= ((0x3 << (0 * 4)) | (0x3 << (1 * 4)) | (0x3 << (2 * 4)) |
                   (0x3 << (3 * 4)) | (0x3 << (4 * 4)) | (0x3 << (5 * 4)));

    delay_ms(20);
    lcd_send_nibble(0x03); delay_ms(5);
    lcd_send_nibble(0x03); delay_ms(1);
    lcd_send_nibble(0x03); delay_ms(1);
    lcd_send_nibble(0x02);

    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
    delay_ms(5);
}

void lcd_load_battery_chars() {
    for (int i = 0; i < 5; i++) {
        lcd_command(0x40 + (i * 8));
        delay_ms(1);
        for (int j = 0; j < 8; j++) {
            lcd_data(battery_levels[i][j]);
        }
    }
    lcd_command(0x80);
    delay_ms(1);
}

void battery_animation() {
    while (1) {
        for (int i = 0; i < 5; i++) {
            lcd_command(0xC0);
            lcd_data(i);
            delay_ms(50);
        }
        for (int i = 4; i >= 0; i--) {
            lcd_command(0xC0);
            lcd_data(i);
            delay_ms(50);
        }
    }
}
