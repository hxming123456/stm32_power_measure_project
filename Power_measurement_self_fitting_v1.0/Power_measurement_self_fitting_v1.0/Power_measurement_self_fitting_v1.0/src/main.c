//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "diag/Trace.h"
#include "bsp_usart.h"
#include "bsp_tim.h"
#include "Data_pool.h"
#include "iic_2864_operate.h"
#include "Fitting_method.h"
#include "Clib.h"
// ----------------------------------------------------------------------------
//
// Standalone STM32F1 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"



ErrorStatus HSEStartUpStatus;

uint8_t key_status = 1;
uint8_t key_backup = 1;
uint32_t down_time = 0;
uint32_t up_time = 0;
uint8_t relay_sta = 1;

void system_clk_init(void)
{
	RCC_DeInit();

	RCC_HSEConfig(RCC_HSE_ON);

	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus)
	{
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		RCC_PCLK2Config(RCC_HCLK_Div1);
		RCC_PCLK1Config(RCC_HCLK_Div2);
	}

	RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
	RCC_PLLCmd(ENABLE);

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
}

void relay_pin_init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
#if 1
uint32_t Get_pvi(uint8_t *data,uint32_t len,float *p,float *v,float *i)
{
	uint32_t tmp_p=0,tmp_v=0,tmp_i=0;
	uint32_t tmp = 0;

	uint32_t p_coe=5046000;
	uint32_t v_coe=183200;
	uint32_t i_coe=15695;

	float ret_p=0.0f,ret_v=0.0f,ret_i=0.0f;
	float p_low=250.0f;

	//if(len != 24)
	//{
	//	return 0;
	//}

	if(*data == 0xf2)
	{
		return 2;
	}

	tmp_p = (((data[17]<<8)|data[18])<<8)|data[19];
	tmp_v = (((tmp|data[5]<<8)|data[6])<<8)|data[7];
	tmp_i = (((data[11]<<8)|data[12])<<8)|data[13];

	ret_p = (float)(p_coe)/(tmp_p);
	ret_v = (float)(v_coe)/(tmp_v);
	ret_i = (float)(i_coe)/(tmp_i);

	if(ret_p > p_low)
	{
		Debug_usart_write("come high\r\n",10,'Y');
		*p = ret_p*0.7453f+6.0401f;
		*p -= ((*p/100)*1);
	}
	else
	{
		Debug_usart_write("come low\r\n",10,'Y');
		*p = ret_p*0.7431f+0.7265f;
		*p -= ((*p/100)*1.4f);
	}
	*v = ret_v*0.0218f+215.27f;
	*i = ret_i*0.7356f+0.0164f;
	*i -= (*i/300);

	return 1;
}
#endif

uint32_t Get_float_mantissa_len(float *data)
{
	float len_2 = 10.0;
	float len_3 = 100.0;
	float len_4 = 1000.0;

	if(*data < len_2)
			return 1;
	else if(*data > len_2 && *data < len_3)
			return 2;
	else if(*data > len_3 && *data < len_4)
			return 3;
	else
			return 4;
}

void write_data_to_nextion(uint8_t type,uint8_t *data,uint32_t len)
{
	uint8_t data_tail[3] = {0xff,0xff,0xff};

	if(type==P_T0_CONTROL_TYPE)
	{
		Nextion_usart_write((uint8_t*)"t0.txt=\"",8);
		Nextion_usart_write(data,len);
		Nextion_usart_write((uint8_t*)"\"",1);
	}
	else if(type==V_T1_CONTROL_TYPE)
	{
		Nextion_usart_write((uint8_t*)"t1.txt=\"",8);
		Nextion_usart_write(data,len);
		Nextion_usart_write((uint8_t*)"\"",1);
	}
	else if(type==I_T2_CONTROL_TYPE)
	{
		Nextion_usart_write((uint8_t*)"t2.txt=\"",8);
		Nextion_usart_write(data,len);
		Nextion_usart_write((uint8_t*)"\"",1);
	}
	else if(type==STA_CONTROL_TYPE)
	{
		Nextion_usart_write((uint8_t*)"t7.txt=\"",8);
		Nextion_usart_write(data,len);
		Nextion_usart_write((uint8_t*)"\"",1);
	}

	Nextion_usart_write(data_tail,3);
}

void key_operate(void)
{
	if(key_status != key_backup)
	{
	    if(key_status == 0)
	    {
	    	if(relay_sta==1)
	    		GPIO_SetBits(GPIOC,GPIO_Pin_0);
	    	else
	    		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
	    	relay_sta = ~relay_sta;
	    }
	    key_backup = key_status;
	}
}

void lcd_show_init(void)
{
	iic_2864_Puts(0,0,(uint8_t *)"P:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"V:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"I:",&Font_11x18,(uint8_t)COLOR_WHITE);

	iic_2864_Puts(10,0,(uint8_t *)"W",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(10,1,(uint8_t *)"V",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(10,2,(uint8_t *)"A",&Font_11x18,(uint8_t)COLOR_WHITE);
}



int main(int argc, char* argv[])
{
	//float x[11] = {
	//		50.328392,98.841822,482.389191,528.663540,576.445003,949.242083,
	//		998.171312,1410.955474,1455.070864,1869.162072,2322.495958};
	//float y[11] = {52,101.83,491.36,539.53,587.22,968.73,1017.53,
	//			  1447.5,1485.93,1914.72,2381.25};

	float x[11] = {0.226856,0.444463,2.163232,2.377381,2.589446,4.273389,4.493904,
					6.365462,6.563147,8.465073,10.510610};
	float y[11] = {0.26,0.47,2.24,2.46,2.68,4.4,4.64,6.58,6.67,8.71,10.84};


	uint8_t a_buf[10] = {0};
	uint8_t b_buf[10] = {0};
	uint8_t r_buf[10] = {0};

	float a = 0.0f;
	float b = 0.0f;
	float r = 0.0f;

	uint8_t a_index=0;
	uint8_t b_index=0;

	system_clk_init();

	//CSE7766_usart_init();
	//AC6530_usart_init();
	//Nextion_usart_init();
	Debug_usart_init();

	//relay_pin_init();
	//tim_init();
	//initDataPool(&cse7766rx);
	//initDataPool(&ac6530rx);

	iic_2864_ssd1306_init();

	//lcd_show_init();

	//iic_2864_Puts(2,0,(uint8_t *)"123456",&Font_11x18,(uint8_t)COLOR_WHITE);

	Get_coe_a_b_r(x,y,11,&a,&b,&r);
	if(a < 0)
	{
		a = -a;
		iic_2864_Puts(0,0,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		a_index++;
	}
	if(b < 0)
	{
		b = -b;
		iic_2864_Puts(0,1,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		b_index++;
	}
	flodou_to_string(a,a_buf,1,5);
	flodou_to_string(b,b_buf,1,5);
	flodou_to_string(r,r_buf,1,5);
	iic_2864_Puts(a_index,0,a_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(b_index,1,b_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,r_buf,&Font_11x18,(uint8_t)COLOR_WHITE);


	while (1)
    {
		key_operate();
    }
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
