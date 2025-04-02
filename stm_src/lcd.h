#ifndef LCD_H
#define LCD_H

#include "stm32f103xb.h"
#include <stdint.h>

extern uint8_t battery_levels[5][8];

void lcd_init();
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_load_battery_chars();
void battery_animation();

#endif // LCD_H
