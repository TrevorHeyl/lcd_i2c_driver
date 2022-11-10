# LCD I2C Driver Library

Driver for Alphanumeric Character displays based upon the HD44780 driver but using the I2C to IO expander IC PCF8574T

The Pin mappings from I2C byte on the PCF8575T to LCD pins are particular to a popular unbranded I2C to LCD piggyback adapter available on the market
 
How to use this driver:  
1. A Shim abstraction is provided in lcd_i2c_shim.c and .h this module is used to hook your own i2c write and microsecond delay functions, making it more protable,  *  update the shim module first
2. In you application , include lcd_ic module
3. Create a lcd_display_config variable to configure your LCD display and call lcd_init(&my_lcd_config);
4. Call the lcd_clear, or lcd_print_at or other inerface functions as required

Some credit goes to fm4dd for a few parts of the initialisation code, fm4dd has no copyright or license requirements

PCF8574T I2C adapter module uses 4-bit mode, LCD pins D0-D3 are not used.    
Adapter module wiring:                                                 
                                                                         
  PCF8574T     LCD                                                       
  ========     ======                                                    
     P0        RS                                                        
     P1        RW                                                        
     P2        Enable                                                    
     P3        Led Backlight                                             
     P4        D4                                                        
     P5        D5                                                        
     P6        D6                                                        
     P7        D7                                                        
                                                                         
 I2C-byte: D7 D6 D5 D4 BL EN RW RS                                       
