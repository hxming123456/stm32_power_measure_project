#ifndef __IIC_2864_GPIO_SIMULATE_H
#define __IIC_2864_GPIO_SIMULATE_H

#include "lcd_show_content.h"
#include "stm32f10x.h"

#define I2C_SCL_HIGH         GPIO_SetBits(GPIOB,GPIO_Pin_8);//GPIOB->BRR=GPIO_Pin_8;
#define I2C_SCL_LOW          GPIO_ResetBits(GPIOB,GPIO_Pin_8);//GPIOB->BSRR=GPIO_Pin_8;

#define I2C_SDA_HIGH         GPIO_SetBits(GPIOB,GPIO_Pin_9);//GPIOB->BRR=GPIO_Pin_9;
#define I2C_SDA_LOW      	 GPIO_ResetBits(GPIOB,GPIO_Pin_9);//GPIOB->BSRR=GPIO_Pin_9;

#define I2C_SCL_read      	 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_8)
#define I2C_SDA_read      	 GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)

void I2C_gpio_init();
void I2C_Delay(uint32_t time);
void I2C_Start(void);
void I2C_Stop(void);
uint8_t I2C_WaitAck(void);
void I2C_SendByte(uint8_t Byte);
uint8_t I2C_Send_one_command(uint8_t Cmd);
uint8_t I2C_Send_one_data(uint8_t Data);
uint8_t I2C_Send_more_data(uint8_t *data,uint8_t count);

#endif
