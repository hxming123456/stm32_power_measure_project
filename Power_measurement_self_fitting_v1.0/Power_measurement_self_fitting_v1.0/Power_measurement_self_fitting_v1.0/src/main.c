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
	uint8_t buf[500] = {0};
	uint32_t len = 24;
	uint8_t str[10] = {0};
	uint8_t empy[2] = {0};
	uint32_t l = 0;
	uint32_t ret = 0;
	uint8_t data[24] = {0xf2,0x5a,0x02,0xcb,0xa0,0x00,0x03,0x3d,0x00,0x3d,
						0x4f,0x00,0x3b,0x03,0x4c,0xfe,0xf0,0x00,0xad,0xca,
						0x61,0x00,0x04,0x29};
	float p_7766=0,v_7766=0,i_7766=0;
	uint8_t p_buf[20] = {0};
	uint8_t v_buf[20] = {"nihao"};
	uint8_t i_buf[20] = {0};

	system_clk_init();

	CSE7766_usart_init();
	AC6530_usart_init();
	Nextion_usart_init();
	Debug_usart_init();

	relay_pin_init();
	tim_init();
	initDataPool(&cse7766rx);
	initDataPool(&ac6530rx);

	iic_2864_ssd1306_init();

	lcd_show_init();

#if 1
	while (1)
    {
		key_operate();
		len = pool_recv_one_command(&cse7766rx,buf,500,CSE_7766_POOL);
		//Debug_usart_write(buf,len,'Y');
#if 1
		if(count_time_flag==1)
		{
			ret = Get_pvi(buf,len,&p_7766,&v_7766,&i_7766);
			Debug_usart_write(&ret,4,'Y');
			if(relay_sta==1)//power close
			{
				ret = 4;
				write_data_to_nextion(P_T0_CONTROL_TYPE,empy,2);
				write_data_to_nextion(V_T1_CONTROL_TYPE,empy,2);
				write_data_to_nextion(I_T2_CONTROL_TYPE,empy,2);
				write_data_to_nextion(STA_CONTROL_TYPE,(uint8_t  *)"RELAY_POWER CLOSE",17);
			}
			if(ret==1)
			{
				flodou_to_string(p_7766,p_buf,Get_float_mantissa_len(&p_7766),2);
				flodou_to_string(v_7766,v_buf,Get_float_mantissa_len(&v_7766),2);
				flodou_to_string(i_7766,i_buf,Get_float_mantissa_len(&i_7766),2);
#if 1

				write_data_to_nextion(P_T0_CONTROL_TYPE,p_buf,str_len(p_buf));
				write_data_to_nextion(V_T1_CONTROL_TYPE,v_buf,str_len(v_buf));
				write_data_to_nextion(I_T2_CONTROL_TYPE,i_buf,str_len(i_buf));
				write_data_to_nextion(STA_CONTROL_TYPE,(uint8_t *)"MEASURE OK",10);

				Debug_usart_write("P:",2,'Y');
				Debug_usart_write(p_buf,str_len(p_buf),'Y');
				Debug_usart_write("\r\nV:",4,'Y');
				Debug_usart_write(v_buf,str_len(v_buf),'Y');
				Debug_usart_write("\r\nI:",4,'Y');
				Debug_usart_write(i_buf,str_len(i_buf),'Y');
				Debug_usart_write("\r\n",2,'Y');
#endif
			}
			else if(ret == 2)
			{
				write_data_to_nextion(STA_CONTROL_TYPE,(uint8_t *)"EXCEPT CODE:0xF2",16);
				Debug_usart_write("Packet head error:0xf2\r\n",24,'Y');
			}
			else if(ret == 0)
			{
				Debug_usart_write("Packet len error\r\n",19,'Y');
			}
		}
#endif
#if 1
		//Debug_usart_write("come in\r\n",9,'Y');
		l = pool_recv_one_command(&ac6530rx,str,10,AC_6530_POOL);
	//	Debug_usart_write(str,10,'Y');
		if(l>0)
		{
			if(ret==1)
			{
				if(strncmp(str,"getv",4)==0)
				{
					AC6530_usart_write(v_buf,str_len(v_buf));
					//Debug_usart_write(v_buf,str_len(v_buf),'Y');
				}
				else if(strncmp(str,"geti",4)==0)
				{
					AC6530_usart_write(i_buf,str_len(i_buf));
					//Debug_usart_write(i_buf,str_len(i_buf),'Y');
				}
				else if(strncmp(str,"getp",4)==0)
				{
					AC6530_usart_write(p_buf,str_len(p_buf));
					//Debug_usart_write(p_buf,str_len(p_buf),'Y');
				}
			}
			else if(ret == 2)
			{
				AC6530_usart_write("Packet head error:0xf2\r\n",24);
			}
			else if(ret == 4)
			{
				AC6530_usart_write("RELAY_POWER CLOSE\r\n",19);
			}
			if(strncmp(str,"getall",6)==0)
			{
				AC6530_usart_write(buf,24);
			}
		}
#endif
		count_time_flag = 0;

    }
#endif
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
