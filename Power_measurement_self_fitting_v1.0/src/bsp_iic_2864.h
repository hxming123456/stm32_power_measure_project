#ifndef 	__BSP_IIC_2864_H
#define	__BSP_IIC_2864_H

#include "stm32f10x.h"

#define IIC_TIMEOUT    			20000
#define I2C_TRANSMITTER_MODE   0
#define I2C_RECEIVER_MODE      1
#define I2C_ACK_ENABLE         1
#define I2C_ACK_DISABLE        0

void iic_2864_gpio_init(void);
void iic_2854_mode_init(void);
void iic_2864_init(void);
uint8_t iic_is_2864device_connect(I2C_TypeDef *iicx,uint8_t address);
uint32_t iic_2864_start(I2C_TypeDef *iicx,uint8_t address);
uint32_t ii2_2864_stop(I2C_TypeDef *iicx);
uint32_t iic_2864_write_byte(I2C_TypeDef* iicx, uint8_t data);
uint32_t iic_2864_write_one_data(I2C_TypeDef* iicx, uint8_t address, uint8_t reg, uint8_t data);
void iic_2864_write_more_data(I2C_TypeDef* I2Cx, uint8_t address, uint8_t reg, uint8_t* data, uint16_t count);


#endif
