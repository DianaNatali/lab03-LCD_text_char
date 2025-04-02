#include "lcd.h"

int main() {
    lcd_init();
    lcd_load_battery_chars();

    lcd_data('B');
    lcd_data('a');
    lcd_data('t');
    lcd_data('e');
    lcd_data('r');
    lcd_data('i');
    lcd_data('a');
    lcd_data(' ');
    lcd_data('S');
    lcd_data('T');
    lcd_data('M');
    lcd_data('3');
    lcd_data('2');

    lcd_command(0x80);
    battery_animation();

    while (1);
}
