#include "bsp_tim.h"
#include "Data_pool.h"

int32_t cnt_time = 0;

uint32_t recv_timeout_start = 0;
uint32_t recv_timeout_end = 0;

uint32_t recv_timeout = 0;

uint32_t pool_wait_time = 0;



void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

	TIM_TimeBaseStructure.TIM_Period = 2000;

	TIM_TimeBaseStructure.TIM_Prescaler = 71;

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;

	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM2, TIM_FLAG_Update);

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);

    TIM_Cmd(TIM2, ENABLE);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}

void TIM2_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void tim_init(void)
{
	TIM2_Configuration();
	TIM2_NVIC_Configuration();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);
}

void delay_ms(int32_t mscnt)
{
	cnt_time = 0;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

	while(1)
	{
		if(cnt_time==mscnt)
		{
			break;
		}
	}

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}

void delay_s(int32_t scnt)
{
	cnt_time = 0;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

	while(1)
	{
		if(cnt_time==(scnt*1000))
		{
			break;
		}
	}

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , DISABLE);
}

void key_scan(void)
{
	if(RELAY_CONTR_KEY_READ==0 || SELF_ADJUST_KEY_READ==0 || EXTERNAL_KEY_READ==0)
	{
	    down_time++;
	}
	else
	{
	    up_time++;
	}

	if(down_time > 20)
	{
	    down_time = 0;
	    key_status = 0;
	}
	if(up_time > 20)
	{
	    up_time = 0;
	    key_status = 1;
	}
}

void TIM2_IRQHandler(void)
{
	static uint32_t count_ms = 0;

	if (TIM_GetITStatus(TIM2 , TIM_IT_Update) != RESET )
	{
		if(count_time_flag==0)
		{
			count_ms++;
			if(count_ms == MAIN_COUNT_TIME_VAL)
			{
				count_ms = 0;
				count_time_flag = 1;
			}
		}
		if(recv_timeout_start==1)
		{
			if(recv_timeout < pool_wait_time)
			{
				recv_timeout++;
			}
			else
			{
				recv_timeout_start = 0;
				recv_timeout_end = 1;
			}
		}
		else
		{
			recv_timeout = 0;
		}

		key_scan();
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
	}
}
