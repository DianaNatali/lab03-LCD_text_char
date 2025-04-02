#include <stdint.h>

#define DR_REG_GPIO_BASE       0x3FF44000
#define GPIO_ENABLE_REG        (DR_REG_GPIO_BASE + 0x20)
#define GPIO_OUT_REG           (DR_REG_GPIO_BASE + 0x04)
#define GPIO_OUT_W1TS_REG      (DR_REG_GPIO_BASE + 0x08)
#define GPIO_OUT_W1TC_REG      (DR_REG_GPIO_BASE + 0x0C)

#define LCD_RS  21
#define LCD_EN  22
#define LCD_D4  19
#define LCD_D5  18
#define LCD_D6  5
#define LCD_D7  4

volatile uint32_t *gpio_enable = (volatile uint32_t *)GPIO_ENABLE_REG;
volatile uint32_t *gpio_out_w1ts = (volatile uint32_t *)GPIO_OUT_W1TS_REG;
volatile uint32_t *gpio_out_w1tc = (volatile uint32_t *)GPIO_OUT_W1TC_REG;

uint8_t battery_levels[5][8] = {
    { 0b01110, 0b11111, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111, 0b00000 },
    { 0b01110, 0b11111, 0b10001, 0b10001, 0b10001, 0b11111, 0b11111, 0b00000 },
    { 0b01110, 0b11111, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111, 0b00000 },
    { 0b01110, 0b11111, 0b10001, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000 },
    { 0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b00000 }
};

void delay_ms(uint32_t ms) {
    for (volatile uint32_t i = 0; i < ms * 1000; i++);
}

void my_gpio_set_level(uint8_t pin, uint8_t level) {
    if (level)
        *gpio_out_w1ts = (1 << pin);
    else
        *gpio_out_w1tc = (1 << pin);
}

void lcd_pulse_enable() {
    my_gpio_set_level(LCD_EN, 1);
    delay_ms(1);
    my_gpio_set_level(LCD_EN, 0);
    delay_ms(1);
}

void lcd_send_nibble(uint8_t nibble) {
    my_gpio_set_level(LCD_D4, (nibble >> 0) & 1);
    my_gpio_set_level(LCD_D5, (nibble >> 1) & 1);
    my_gpio_set_level(LCD_D6, (nibble >> 2) & 1);
    my_gpio_set_level(LCD_D7, (nibble >> 3) & 1);
    lcd_pulse_enable();
}

void lcd_command(uint8_t cmd) {
    my_gpio_set_level(LCD_RS, 0);
    lcd_send_nibble(cmd >> 4);
    lcd_send_nibble(cmd & 0x0F);
    delay_ms(2);
}

void lcd_data(uint8_t data) {
    my_gpio_set_level(LCD_RS, 1);
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
    delay_ms(2);
}

void lcd_init() {
    *gpio_enable |= (1 << LCD_RS) | (1 << LCD_EN) | 
                    (1 << LCD_D4) | (1 << LCD_D5) | 
                    (1 << LCD_D6) | (1 << LCD_D7);

    delay_ms(50);
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
    for (int lvl = 0; lvl < 5; lvl++) {
        lcd_command(0x40 + (lvl * 8));
        delay_ms(1);
        for (int i = 0; i < 8; i++) {
            lcd_data(battery_levels[lvl][i]);
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
            delay_ms(200);
        }
        for (int i = 4; i >= 0; i--) {
            lcd_command(0xC0);
            lcd_data(i);
            delay_ms(200);
        }
    }
}

void app_main() {
    lcd_init();
    lcd_load_battery_chars();
    lcd_data('H');
    lcd_data('o');
    lcd_data('l');
    lcd_data('a');

    battery_animation();
}
