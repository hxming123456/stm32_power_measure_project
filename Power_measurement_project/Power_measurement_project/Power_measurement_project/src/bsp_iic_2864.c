#include "bsp_iic_2864.h"


void iic_2864_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void iic_2854_mode_init(void)
{
	I2C_InitTypeDef I2C_InitStructure;

	I2C_InitStructure.I2C_ClockSpeed = 50000;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x78;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}

uint8_t iic_is_2864device_connect(I2C_TypeDef *iicx,uint8_t address)
{
	uint8_t ret = 0;

	if (!iic_2864_start(iicx, address))
	{
		ret = 1;
	}

	ii2_2864_stop(iicx);

	return ret;
}

uint32_t iic_2864_start(I2C_TypeDef *iicx,uint8_t address)
{
	I2C_GenerateSTART(iicx,ENABLE);
	Debug_usart_write("gen\r\n",5,'Y');
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_MODE_SELECT));
	Debug_usart_write("addr\r\n",6,'Y');
	I2C_Send7bitAddress(iicx,address,I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	return 0;
}

uint32_t ii2_2864_stop(I2C_TypeDef *iicx)
{
	I2C_GenerateSTOP(iicx,ENABLE);
	return 0;
}


uint32_t iic_2864_write_one_data(I2C_TypeDef* iicx, uint8_t address, uint8_t reg, uint8_t data)
{
	Debug_usart_write("begin\r\n",7,'Y');
	iic_2864_start(iicx, address);
	Debug_usart_write("stary\r\n",7,'Y');
	I2C_SendData(iicx, reg);
	Debug_usart_write("reg\r\n",5,'Y');
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(iicx, data);
	Debug_usart_write("data\r\n",6,'Y');
	while(!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	ii2_2864_stop(iicx);
	Debug_usart_write("stop\r\n",7,'Y');
	return 0;
}

void iic_2864_init()
{
	iic_2864_gpio_init();
	iic_2854_mode_init();
}
