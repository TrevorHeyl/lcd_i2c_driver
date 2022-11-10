#ifndef _LCD_I2C_SHIM_
#define  _LCD_I2C_SHIM_
/**
 * @file lcd_i2c_shim.h
 * @author your name T HEYL
 * @brief 
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright M.I.T 2022
 * 
 */
#include <stdint.h>


void lcd_i2c_us_sleep(uint32_t delay);
int lcd_i2c_device_write(void * device , uint8_t * data, uint8_t device_addr);
#endif

