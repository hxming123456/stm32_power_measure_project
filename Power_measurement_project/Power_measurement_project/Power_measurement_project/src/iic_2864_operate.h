#ifndef __IIC_2864_OPERATE_H
#define __IIC_2864_OPERATE_H

#include "stm32f10x.h"
#include "bsp_iic_2864.h"
#include "iic_2864_gpio_simulate.h"
#include "stdlib.h"
#include "string.h"

//#define SSD1306_GPIO_INIT()				   		iic_2864_init()
//#define SSD1306_WRITECOMMAND(command)      		iic_2864_write_one_data(I2C1, 0x78, 0x00, (command))
//#define SSD1306_WRITEDATA(data)            		iic_2864_write_one_data(I2C1, 0x78, 0x40, (data))

#define SSD1306_GPIO_INIT()				 	 I2C_gpio_init()
#define SSD1306_WRITECOMMAND(command)      	 I2C_Send_one_command(command)
#define SSD1306_WRITEDATA(data)            	 I2C_Send_one_data(data)

#define ABS(x)   ((x) > 0 ? (x) : -(x))

#define COLOR_BLACK  0x00
#define COLOR_WHITE  0x01

#define SSD1306_I2C_ADDR         0x78
#define SSD1306_WIDTH            128
#define SSD1306_HEIGHT           64

uint8_t iic_2864_ssd1306_init(void);
void iic_2864_ssd1306_fill(uint8_t color);
void iic_2864_updatescreen(uint8_t data);
void iic_2864_Delay(uint32_t time);
uint32_t iic_2864_setxy(uint8_t x,uint8_t y);

#endif
