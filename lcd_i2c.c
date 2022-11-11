/**
 * @file lcd_i2c.c
 * @author your name T HEYL
 * @brief 
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright M.I.T 2022
 * 
 */
#include "lcd_i2c.h"
#include "lcd_i2c_shim.h"


/**
 * @brief I2C LCD driver
 * Driver for Alphanumeric Character displays based upon the HD44780 driver
 * but using the I2C to IO expander IC PCF8574T
 * The Pin mappings from I2C byte on the PCF8575T to LCD pins are particular
 * to a popular unbranded I2C to LCD piggyback adapter available on the market
 * 
 * How to use this driver:
 * 1) A Shim abstraction is provided in lcd_i2c_shim.c and .h this module is used to 
 *  hok your own i2c write and microsecond delay functions, making it more protable, 
 *  update the shim module first
 * 2) In you application , include lcd_ic module
 * 3) Create a lcd_display_config variable to configure your LCD display and call lcd_init(&my_lcd_config);
 * 4) Call the lcd_clear, or lcd_print_at or other inerface functions as required
 * 
 * Some credit goes to fm4dd for a few parts of the initialisation code
 * 
  
 */

/*------------------------------------------------------------------------ * 
 * PCF8574T I2C adapter module uses 4-bit mode, LCD pins D0-D3 are not used.  *
 * adapter module wiring:                                                 *
 *                                                                         *
 *  PCF8574T     LCD                                                       *
 *  ========     ======                                                    *
 *     P0        RS                                                        *
 *     P1        RW                                                        *
 *     P2        Enable                                                    *
 *     P3        Led Backlight                                             *
 *     P4        D4                                                        *
 *     P5        D5                                                        *
 *     P6        D6                                                        *
 *     P7        D7                                                        *
 *                                                                         *
 * I2C-byte: D7 D6 D5 D4 BL EN RW RS                                       *
 * ----------------------------------------------------------------------- */

lcd_display_config lcd_config = { 	.max_columns=8, 
									.max_rows=1, 
                                    .device_addr=0,
									.bits.auto_increment_cursor=0, 
									.bits.backlight_on = 0,
									.bits.cursor_blink_on = 0,
									.bits.font_is_5x10 = 0,
									.bits.interface_8_bit = 0
									 };



/**
 * @brief _i2c_send_bytes
 *  Send a byte to I2C port with given I2C bus address
 * @param b 
 * @return int 1 = succeeded, 0 = fail
 */
int _i2c_send_byte(uint8_t b, uint8_t device_addr)
{

	int retries = 3;
	int ret = -1;
	uint8_t data[1];

	while( ret != 0 && retries > 0)
	{
		data[0] = b;  
		ret = lcd_i2c_device_write(lcd_config.device , data, device_addr);
		retries--;
        if (ret != 0)
        {
		    lcd_i2c_us_sleep(2000);
        }
	}

	return (ret==0?1:0);
}


/**
 * @brief Print a single character a the current cursor position
 * 
 * @param c 
 */
void lcd_print_char(char c)
{
   uint8_t backlight = (((uint8_t)lcd_config.bits.backlight_on)<<3);

   _i2c_send_byte( ((c&0xf0) | (0b00000101) ), lcd_config.device_addr ); 
   _i2c_send_byte( ((c&0xf0) | (0b00000001) ), lcd_config.device_addr ); 
   _i2c_send_byte( (((c<<4)&0xf0) | (0b00000101)  ), lcd_config.device_addr); 
   _i2c_send_byte( (((c<<4)&0xf0) | (0b00000001) | backlight ), lcd_config.device_addr); 

}


void lcd_backlight_set(uint8_t bl)
{
   lcd_config.bits.backlight_on = bl==1?1:0;
}

void lcd_clear(void)
{
   uint8_t backlight = (((uint8_t)lcd_config.bits.backlight_on)<<3);

   _i2c_send_byte(0b00000100 | backlight, lcd_config.device_addr); //
   _i2c_send_byte(0b00000000 | backlight, lcd_config.device_addr); // d7-d4=0
   _i2c_send_byte(0b00010100 | backlight, lcd_config.device_addr); //
   _i2c_send_byte(0b00010000 | backlight, lcd_config.device_addr); // d0=display clear

}

void lcd_print( char* c)
{
	while(*c) {
		lcd_print_char(*c);
		c++;
	}
}

/**
 * @brief lcd_set_display_address
 *      Set the address where the next display data write will place a display character
 * @param a     Display memery address
 *              00-0F is line 1
 *              40-4f is line 2
 */
void lcd_set_display_address(uint8_t a)
{
   uint8_t backlight = (((uint8_t)lcd_config.bits.backlight_on)<<3);

   _i2c_send_byte( (a&0x70)  | 0b10000100 | backlight, lcd_config.device_addr); 
   _i2c_send_byte( (a&0x70)  | 0b00000000 | backlight, lcd_config.device_addr); 
   _i2c_send_byte( ((a<<4)&0xf0) | 0b00000100 | backlight, lcd_config.device_addr); 
   _i2c_send_byte( ((a<<4)&0xf0) |0b00000000 | backlight, lcd_config.device_addr); 
}


/**
 * @brief lcd_print_at
 *      Print on the LCD at a specific row and column
 *      Rows start at 1 for first row and columns start at 1 for first column
 * @param c         Null terminated string to print
 * @param row       Row number starting from 1 for 1st row
 * @param column    Column number starting from 1 for column 1
 */
void lcd_print_at( char* c, uint8_t row,uint8_t column)
{
    uint8_t dd_address;

    if( (row < (lcd_config.max_rows + 1)) && (column < (lcd_config.max_columns +1)) ) 
    {
        dd_address = (row-1)* 0x40 + (column-1);
        lcd_set_display_address(dd_address);
        lcd_print(c);
    }


}

/**
 * @brief lcd_init
 *  Setup and initialise the display, set the mode, line count, character type, cursor etc.
 * @param dev 
 */
void lcd_init(  lcd_display_config *cfg )
{
   
   uint8_t backlight = (((uint8_t)cfg->bits.backlight_on)<<3);

   lcd_config.max_columns = cfg->max_columns;
   lcd_config.max_rows = cfg->max_rows;
   lcd_config.device_addr = cfg->device_addr;
   lcd_config.device = cfg->device;
   lcd_config.bits.backlight_on = cfg->bits.backlight_on;
   lcd_config.bits.auto_increment_cursor = cfg->bits.auto_increment_cursor;
   lcd_config.bits.cursor_blink_on = cfg->bits.cursor_blink_on;
   lcd_config.bits.cursor_on = cfg->bits.cursor_on;
   lcd_config.bits.font_is_5x10 = cfg->bits.font_is_5x10;
   lcd_config.bits.interface_8_bit = cfg->bits.interface_8_bit;

   lcd_i2c_us_sleep(15000);             // wait 15msec
   _i2c_send_byte(0b00110100, lcd_config.device_addr); // d7=0, d6=0, d5=1, d4=1, RS,RW=0 EN=1
   _i2c_send_byte(0b00110000, lcd_config.device_addr); // d7=0, d6=0, d5=1, d4=1, RS,RW=0 EN=0
   lcd_i2c_us_sleep(4100);              // wait 4.1msec
   _i2c_send_byte(0b00110100, lcd_config.device_addr); // 
   _i2c_send_byte(0b00110000, lcd_config.device_addr); // same
   lcd_i2c_us_sleep(100);               // wait 100usec
   _i2c_send_byte(0b00110100, lcd_config.device_addr); //
   _i2c_send_byte(0b00110000, lcd_config.device_addr); // 8-bit mode init complete
   lcd_i2c_us_sleep(4100);              // wait 4.1msec
   _i2c_send_byte(0b00100100, lcd_config.device_addr); //
   _i2c_send_byte(0b00100000, lcd_config.device_addr); // switched now to 4-bit mode

   /* -------------------------------------------------------------------- *
    * 4-bit mode initialization complete. Now configuring the function set *
    * -------------------------------------------------------------------- */
   lcd_i2c_us_sleep(40);                // wait 40usec
   _i2c_send_byte(0b00100100, lcd_config.device_addr); //
   _i2c_send_byte(0b00100000, lcd_config.device_addr); // keep 4-bit mode
   _i2c_send_byte(0b10000100, lcd_config.device_addr); //
   _i2c_send_byte(0b10000000, lcd_config.device_addr); // D3=2lines, D2=char5x8

   /* -------------------------------------------------------------------- *
    * Next turn display off                                                *
    * -------------------------------------------------------------------- */
   lcd_i2c_us_sleep(40);                // wait 40usec
   _i2c_send_byte(0b00000100, lcd_config.device_addr); //
   _i2c_send_byte(0b00000000, lcd_config.device_addr); // D7-D4=0
   _i2c_send_byte(0b10000100, lcd_config.device_addr); //
   _i2c_send_byte(0b10000000, lcd_config.device_addr); // D3=1 D2=display_off, D1=cursor_off, D0=cursor_blink

   /* -------------------------------------------------------------------- *
    * Display clear, cursor home                                           *
    * -------------------------------------------------------------------- */
   lcd_i2c_us_sleep(40);                // wait 40usec
   _i2c_send_byte(0b00000100, lcd_config.device_addr); //
   _i2c_send_byte(0b00000000, lcd_config.device_addr); // D7-D4=0
   _i2c_send_byte(0b00010100, lcd_config.device_addr); //
   _i2c_send_byte(0b00010000, lcd_config.device_addr); // D0=display_clear

   /* -------------------------------------------------------------------- *
    * Set cursor direction                                                 *
    * -------------------------------------------------------------------- */
   lcd_i2c_us_sleep(40);                // wait 40usec
   _i2c_send_byte(0b00000100, lcd_config.device_addr); //
   _i2c_send_byte(0b00000000, lcd_config.device_addr); // D7-D4=0
   _i2c_send_byte(0b01100100, lcd_config.device_addr); //
   _i2c_send_byte(0b01100000, lcd_config.device_addr); // print left to right

   /* -------------------------------------------------------------------- *
    * Turn on the display                                                  *
    * -------------------------------------------------------------------- */
   lcd_i2c_us_sleep(40);                // wait 40usec
   _i2c_send_byte(0b00000100, lcd_config.device_addr); //
   _i2c_send_byte(0b00000000, lcd_config.device_addr); // D7-D4=0
   _i2c_send_byte( 0b11000100 | ((cfg->bits.cursor_on)<<5) | ((cfg->bits.cursor_blink_on)<<4) , lcd_config.device_addr); //
   _i2c_send_byte( 0b11001000 | ((cfg->bits.cursor_on)<<5) | ((cfg->bits.cursor_blink_on)<<4 | backlight  ) , lcd_config.device_addr); // D3=1 D2=display_on, D1=cursor_on, D0=cursor_blink	

   lcd_i2c_us_sleep(40);                // wait 40usec

}


