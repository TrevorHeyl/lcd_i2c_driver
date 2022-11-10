/**
 * @file lcd_i2c_shim.c
 * @author your name T HEYL
 * @brief 
 * @version 0.1
 * @date 2022-11-10
 * 
 * @copyright M.I.T 2022
 * 
 */
#include "lcd_i2c_shim.h"

// Include abstraction dependencies here
#include <zephyr/kernel.h> // for k_msleep
#include <zephyr/drivers/i2c.h> // for i2c_write

/**
 * @brief I2C LCD module abstraction layer
 * 
 * Some functionality is needed by the I2C LCD driver including
 * 1. A microsecond delay - preferably non-blocking - lcd_i2c_us_sleep
 * 2. An I2C send byte type function lcd_i2c_device_write
 * Modify this file to effect the support
 *  
 */


/**
 * @brief microsecond delay
 *      User implementation of OS/non-blocking microsecond delay
 * 
 * @param delay in micro seconds
 */
void lcd_i2c_us_sleep(uint32_t delay) 
{
	// Your micrsecond delay goes here
    k_usleep(delay);
}

/**
 * @brief lcd_i2c_device_write
 *  User implementation of an i2c device write function goes here
 * @param device void pointer to the device
 * @param data pointer to byte array to send - only 1 byte is ever written at a time
 * @param device_addr i2c bus address of the device
 * @return int 
 */
int lcd_i2c_device_write(void * device , uint8_t * data, uint8_t device_addr)
{
    return i2c_write(device , data, 1 , device_addr);
}