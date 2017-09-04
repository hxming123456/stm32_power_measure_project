#include "iic_2864_gpio_simulate.h"

void I2C_gpio_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;

	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= GPIO_Pin_9;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructer);
}

void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= GPIO_Pin_9;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructer);
}

void I2C_Delay(uint32_t time)
{
	uint32_t i,j;

	for(i=0;i<time;i++)
	{
		for(j=0;j<254;j++);
	}
}

void I2C_Start(void)
{
	I2C_SDA_HIGH
	I2C_Delay(1);
	I2C_SCL_HIGH
	I2C_Delay(1);
	I2C_SDA_LOW
	I2C_Delay(1);
	I2C_SCL_LOW
}

void I2C_Stop(void)
{
	I2C_SCL_HIGH
    I2C_SDA_LOW;
	I2C_Delay(1);
    I2C_SDA_HIGH;
    I2C_Delay(1);
}

uint8_t I2C_WaitAck(void)  //0:aCK     1:NoAck
{
	int count = 0;

	SDA_IN();
    I2C_SCL_HIGH;

    count = 10000;
    while(count-- > 0)
    {
    	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)==0)
    	{
    		I2C_SCL_LOW
			SDA_OUT();
    		return  0;
    	}
    }

    return 1;
}

void I2C_SendByte(uint8_t Byte)
{
    uint8_t Cnt;

    __set_PRIMASK(1);
    SDA_OUT();
    for(Cnt=0;Cnt<8;Cnt++)
    {
    	I2C_SCL_LOW
		I2C_Delay(1);
        if(Byte&0x80)
            I2C_SDA_HIGH
        else
            I2C_SDA_LOW

        I2C_SCL_HIGH;
        I2C_Delay(1);
        I2C_SCL_LOW;
        Byte <<= 1;
    }
    __set_PRIMASK(0);
}

uint8_t I2C_Send_one_command(uint8_t Cmd)
{
    I2C_Start();
    I2C_SendByte(0x78);
    if(I2C_WaitAck())
    {
    	Debug_usart_write("send 78 error\r\n",15,'Y');
    	return 1;
    }
    I2C_SendByte(0x00);
    if(I2C_WaitAck())
    {
        	Debug_usart_write("send 00 error\r\n",15,'Y');
        	return 2;
    }
    I2C_SendByte(Cmd);
    if(I2C_WaitAck())
    {
        	Debug_usart_write("send cmd error\r\n",16,'Y');
        	return 3;
        }
    I2C_Stop();
    return 0;
}

uint8_t I2C_Send_one_data(uint8_t Data)
{
    I2C_Start();
    I2C_SendByte(0x78);
    if(I2C_WaitAck())
    {
    	 Debug_usart_write("send da78 error\r\n",17,'Y');
    	 return 1;
    }
    I2C_SendByte(0x40);
    if(I2C_WaitAck())
    {
          Debug_usart_write("send 40 error\r\n",15,'Y');
          return 2;
    }
    I2C_SendByte(Data);
    if(I2C_WaitAck())
    {
        Debug_usart_write("send da error\r\n",15,'Y');
        return 3;
    }
    I2C_Stop();
    return 0;
}

uint8_t I2C_Send_more_data(uint8_t *data,uint8_t count)
{
	uint8_t i;

	I2C_Start();
	I2C_SendByte(0x78);
	if(I2C_WaitAck())
	{
	    Debug_usart_write("send da78 error\r\n",17,'Y');
	    return 1;
	}
	I2C_SendByte(0x40);
	if(I2C_WaitAck())
	{
	    Debug_usart_write("send 40 error\r\n",15,'Y');
	    return 2;
	}
	for (i = 0; i < count; i++)
	{
		I2C_SendByte(data[i]);
		if(I2C_WaitAck())
		{
		    Debug_usart_write("send da error\r\n",15,'Y');
		    return 3;
		}
	}

	I2C_Stop();
	return 0;
}
