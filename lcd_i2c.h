#ifndef _DISP_LCD_I2C_
#define _DISP_LCD_I2C_

/**
 * @file lcd_i2c.h
 * @author your name T Heyl 2022 
 * @brief 
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright M.I.T license
 * 
 */
#include <stdint.h>

#define LCD_MAX_ROWS_COUNT 4
#define LCD_MAX_COLUMNS_COUNT 40 

/**
 * @brief lcd I2C device context
 * 
 */
typedef struct lcd_display_config 
{
    uint8_t max_rows;
    uint8_t max_columns;
    uint8_t device_addr;
    void *  device;         // handle/pointer to i2c device driver
    struct bits 
    {
        uint8_t cursor_on               : 1;
        uint8_t cursor_blink_on         : 1;
        uint8_t interface_4_bit         : 1;
        uint8_t backlight_on            : 1;
        uint8_t auto_increment_cursor   : 1;
        uint8_t font_is_5x10            : 1;
        uint8_t spare                   : 2;
    }bits;
    
}lcd_display_config;

void lcd_print_char(char c);
void lcd_clear(void);
void lcd_print( char* c);
void lcd_print_at( char* c, uint8_t row,uint8_t column);
void lcd_set_display_address(uint8_t a);
void lcd_init(  lcd_display_config *config );

#endif

