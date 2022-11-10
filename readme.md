LCD I2C Driver Library

#####################

Driver for Alphanumeric Character displays based upon the HD44780 driver
 but using the I2C to IO expander IC PCF8574T

The Pin mappings from I2C byte on the PCF8575T to LCD pins are particular
to a popular unbranded I2C to LCD piggyback adapter available on the market
 
How to use this driver:
#. A Shim abstraction is provided in lcd_i2c_shim.c and .h this module is used to hook your own i2c write and microsecond delay functions, making it more protable,  *  update the shim module first
#. In you application , include lcd_ic module
#. Create a lcd_display_config variable to configure your LCD display and call lcd_init(&my_lcd_config);
#. Call the lcd_clear, or lcd_print_at or other inerface functions as required

Some credit goes to fm4dd for a few parts of the initialisation code

