#include "bsp_usart.h"
#include "bsp_tim.h"

uint8_t recv_data_flag = 0;
uint32_t count_time_flag = 0;
uint8_t uart4_recv_flag = 0;

void CSE7766_usart_init()//usart1
{
	USART_InitTypeDef USART_InitStructure;

	CSE7766_usart_gpio_init();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	USART_InitStructure.USART_BaudRate = 4800;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_Even;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART1,&USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	CSE7766_usart_nvic_init();

	USART_Cmd(USART1,ENABLE);
}

void AC6530_usart_init()//usart2
{
	USART_InitTypeDef USART_InitStructure;

	AC6530_usart_gpio_init();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART2,&USART_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	AC6530_usart_nvic_init();

	USART_Cmd(USART2,ENABLE);
}

void Nextion_usart_init(void)//usart3
{
	USART_InitTypeDef USART_InitStructure;

	Nextion_usart_gpio_init();

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART3,&USART_InitStructure);

	//USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	//Nextion_usart_nvic_init();

	USART_Cmd(USART3,ENABLE);
}

void CSE7766_usart_change_baud(uint32_t baud)
{
	USART_InitTypeDef USART_InitStructure;

	USART_Cmd(USART1, DISABLE);

	USART_InitStructure.USART_BaudRate = baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(USART1,&USART_InitStructure);

	USART_Cmd(USART1,ENABLE);
}

void CSE7766_usart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void AC6530_usart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Nextion_usart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void CSE7766_usart_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void AC6530_usart_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Nextion_usart_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void CSE7766_usart_write(uint8_t *data,uint32_t data_len)
{
	uint32_t i = 0;
	for(i=0;i<data_len;i++)
	{
		USART_SendData(USART1,data[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	}
}

void AC6530_usart_write(uint8_t *data,uint32_t data_len)
{
	uint32_t i = 0;
	for(i=0;i<data_len;i++)
	{
		USART_SendData(USART2,data[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
	}
}

void Nextion_usart_write(uint8_t *data,uint32_t data_len)
{
	uint32_t i = 0;
	for(i=0;i<data_len;i++)
	{
		USART_SendData(USART3,data[i]);
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);
	}
}

void Debug_usart_init()
{
	USART_InitTypeDef USART_InitStructure;

	Debug_usart_gpio_init();

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_Init(UART4,&USART_InitStructure);

	//USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);

	//Debug_usart_nvic_init();

	USART_Cmd(UART4,ENABLE);

}

void Debug_usart_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Debug_usart_nvic_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Debug_usart_write(void *data,uint32_t data_len,uint8_t debug_type)
{
	uint8_t * buf = (uint8_t *)data;

	uint32_t i = 0;

	if(debug_type == 'Y')
	{
		for(i=0;i<data_len;i++)
		{
			USART_SendData(UART4,buf[i]);
			while(USART_GetFlagStatus(UART4,USART_FLAG_TXE)==RESET);
		}
	}
}

uint32_t pool_recv_one_command(Datapool *pool_type,uint8_t *buf,uint32_t len,uint32_t type)
{
	uint32_t i = 0;
	uint32_t status = 0;
	uint8_t c = 0;

	while(1)
	{
		if(pool_type->stock > 0)
		{
			if(i==len)
			{
				if(read_one_data_to_datapool(pool_type,&c)==0)
				{
					;
				}
			}
			else
			{
				if(read_one_data_to_datapool(pool_type,&buf[i])==0)
				{
					if(type == CSE_7766_POOL)
					{
						if(buf[0] == 0xf2 || buf[0] == 0x55)
						{
							i++;
						}
						else
						{
							i = 0;
						}
						if(i==24)
						{
							break;
						}
					}
					else
					{
						i++;
					}
				}
			}
			recv_timeout_start = 0;
			recv_timeout_end = 0;

			status = 0;
		}
		else if(status == 0)
		{
			recv_timeout_start = 1;
			recv_timeout = 0;
			status++;
		}

		if(status == 1)
		{
			if(recv_timeout_end==1)
			{
				recv_timeout_end = 0;
				break;
			}
		}
	}

	return i;
}

void USART1_IRQHandler(void)//cse7766
{
	uint8_t c;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		c = USART_ReceiveData(USART1);
		write_one_data_to_datapool(&cse7766rx,c);
		//Debug_usart_write(&c,1,'Y');
	}
}

void USART2_IRQHandler(void)//ac6530
{
	uint8_t c;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		c = USART_ReceiveData(USART2);
		write_one_data_to_datapool(&ac6530rx,c);
		//Debug_usart_write(&c,1,'Y');
	}
}

void UART4_IRQHandler(void)
{
	uint8_t c;

	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		c = USART_ReceiveData(UART4);
		write_one_data_to_datapool(&ac6530rx,c);
	}
}
