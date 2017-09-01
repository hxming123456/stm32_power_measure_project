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
#include "bsp_internal_flash.h"
#include "lcd_show_content.h"
#include "WrRe_coe_from_flash.h"
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
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"
//#pragma GCC diagnostic ignored "-Wmissing-declarations"
//#pragma GCC diagnostic ignored "-Wreturn-type"

ErrorStatus HSEStartUpStatus;



uint8_t key_status = 1;
uint8_t key_backup = 1;
uint32_t down_time = 0;
uint32_t up_time = 0;
uint8_t relay_sta = 1;
uint32_t external_wait_flag = 0;

uint8_t operate_mode = 0;
uint8_t self_ad_ok_flag = 0;
uint8_t external_ad_ok_flag = 0;

GPIO_TypeDef* ch_io[16] = {GPIOA,GPIOC,GPIOC,GPIOB,GPIOB,GPIOB,GPIOB,GPIOB,GPIOB,GPIOC,GPIOC,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA};
uint16_t	ch_pin[16] = {GPIO_Pin_8,GPIO_Pin_7,GPIO_Pin_6,GPIO_Pin_15,GPIO_Pin_14,GPIO_Pin_13,GPIO_Pin_12,GPIO_Pin_1,
						  GPIO_Pin_0,GPIO_Pin_5,GPIO_Pin_4,GPIO_Pin_7,GPIO_Pin_6,GPIO_Pin_5,GPIO_Pin_4,GPIO_Pin_1};

double self_adjust_coea_pl = 0;
double self_adjust_coeb_pl = 0;
double self_adjust_coer_pl = 0;

double self_adjust_coea_ph = 0;
double self_adjust_coeb_ph = 0;
double self_adjust_coer_ph = 0;

double self_adjust_coea_i = 0;
double self_adjust_coeb_i = 0;
double self_adjust_coer_i = 0;

double self_adjust_coea_il = 0;
double self_adjust_coeb_il = 0;
double self_adjust_coer_il = 0;

double self_adjust_coea_pl_110 = 0;
double self_adjust_coeb_pl_110 = 0;
double self_adjust_coer_pl_110 = 0;

double self_adjust_coea_ph_110 = 0;
double self_adjust_coeb_ph_110 = 0;
double self_adjust_coer_ph_110 = 0;

double self_adjust_coea_i_110 = 0;
double self_adjust_coeb_i_110 = 0;
double self_adjust_coer_i_110 = 0;

double self_adjust_coea_il_110 = 0;
double self_adjust_coeb_il_110 = 0;
double self_adjust_coer_il_110 = 0;

double self_adjust_comv = 0;
double self_adjust_comv_110 = 0;

double external_adjust_coea_p = 0;
double external_adjust_coeb_p = 0;
double external_adjust_coer_p = 0;

double external_adjust_coea_i = 0;
double external_adjust_coeb_i = 0;
double external_adjust_coer_i = 0;

double external_adjust_comv = 0;

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

void more_relay_pin_init(GPIO_TypeDef *GPIOx,uint16_t pin)
{
#if 1
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOx, &GPIO_InitStructure);
#endif
}

void load_relay_pin_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

#if 1
	more_relay_pin_init(GPIOA,GPIO_Pin_8);//CHO
	more_relay_pin_init(GPIOC,GPIO_Pin_7);
	more_relay_pin_init(GPIOC,GPIO_Pin_6);
	more_relay_pin_init(GPIOB,GPIO_Pin_15);
	more_relay_pin_init(GPIOB,GPIO_Pin_14);
	more_relay_pin_init(GPIOB,GPIO_Pin_13);
	more_relay_pin_init(GPIOB,GPIO_Pin_12);
	more_relay_pin_init(GPIOB,GPIO_Pin_1);
	more_relay_pin_init(GPIOB,GPIO_Pin_0);
	more_relay_pin_init(GPIOC,GPIO_Pin_5);
	more_relay_pin_init(GPIOC,GPIO_Pin_4);
	more_relay_pin_init(GPIOA,GPIO_Pin_7);
	more_relay_pin_init(GPIOA,GPIO_Pin_6);
	more_relay_pin_init(GPIOA,GPIO_Pin_5);
	more_relay_pin_init(GPIOA,GPIO_Pin_4);
	more_relay_pin_init(GPIOA,GPIO_Pin_1);//CH15
#endif

	//more_relay_pin_init(GPIOA,GPIO_Pin_12);
	//GPIO_SetBits(GPIOA,GPIO_Pin_12);
}


void source_relay_pin_init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

uint32_t Check_data_is_error(uint8_t *data,uint32_t len)
{
	uint32_t sum_data = 0;
	uint32_t i = 0;
	uint32_t ret_count = 0;
	uint32_t ret_data = data[23];

	if(len != 24)
	{
		return 0;
	}

	if(*data != 0x55)
	{
		return data[0];
	}

	if((data[20]&0x70) <0x70)
	{
		return data[20];
	}

	for(i=2;i<23;i++)
	{
		sum_data += data[i];
	}

	ret_count = ((sum_data & 0x000000ff));
	//Debug_usart_write(&ret_count,4,'Y');
	//Debug_usart_write(&ret_data,4,'Y');
	if(ret_count==ret_data)
	{
		//Debug_usart_write("check 7766 data ok\r\n",20,INFO_DEBUG);
		return 1;
	}
	else
	{
		//Debug_usart_write("check 7766 data nok\r\n",21,INFO_DEBUG);
		return 0;
	}
}

uint32_t Get_6530_pvi(double *p,double *v,double *i)
{
	uint8_t p_6530[10] = {0};
	uint8_t v_6530[10] = {0};
	uint8_t i_6530[10] = {0};

	uint32_t p_len = 0;
	uint32_t v_len = 0;
	uint32_t i_len = 0;

	uint8_t cnt = 0;

	uint8_t debug = 0;

	uint8_t i_tmp = 'i';
	uint8_t v_tmp = 'v';
	uint8_t p_tmp = 'p';

	uint8_t read_outtime = 0;


	for(cnt=0;cnt<3;cnt++)
	{
		initDataPool(&ac6530rx);
		initDataPool(&externalrx);
		External_usart_write(&i_tmp,1);
		//AC6530_usart_write(&i_tmp,1);
		count_time_flag = 0;
		while(1)
		{
			i_len = pool_recv_one_command(&externalrx,i_6530,10,EXTERNAL_WAIT_TIME);
			//i_len = pool_recv_one_command(&ac6530rx,i_6530,10,AC6530_WAIT_TIME);
			if(i_len > 2)
			{
				break;
			}
			if(count_time_flag==1)
			{
				read_outtime++;
				if(read_outtime >= 3)
				{
					read_outtime = 0;
					return 0;
				}
				count_time_flag = 0;
			}
		}

		Debug_usart_write("read i over:",12,INFO_DEBUG);
		Debug_usart_write(i_6530,i_len,INFO_DEBUG);
		Debug_usart_write("\r\n",2,INFO_DEBUG);
		if(i_len > 0)
		{
			break;
		}
		if((cnt == 2) && (debug==0))
		{
			return 0;
		}
	}


	read_outtime = 0;
	for(cnt=0;cnt<3;cnt++)
	{
		initDataPool(&ac6530rx);
		//AC6530_usart_write(&v_tmp,1);
		External_usart_write(&v_tmp,1);
		count_time_flag = 0;
		while(1)
		{
			v_len = pool_recv_one_command(&externalrx,v_6530,10,EXTERNAL_WAIT_TIME);
			//v_len = pool_recv_one_command(&ac6530rx,v_6530,10,AC6530_WAIT_TIME);
			if(v_len > 2)
			{
				Debug_usart_write(v_6530,v_len,INFO_DEBUG);
				break;
			}
			if(count_time_flag==1)
			{
				read_outtime++;
				if(read_outtime >= 3)
				{
					read_outtime = 0;
					return 0;
				}
				count_time_flag = 0;
			}
		}
		Debug_usart_write("read v over:",12,INFO_DEBUG);
		Debug_usart_write(v_6530,v_len,INFO_DEBUG);
		Debug_usart_write("\r\n",2,INFO_DEBUG);
		if(v_len > 0)
		{
			break;
		}
		if((cnt == 2) && (debug==0))
		{
			return 0;
		}
	}

	read_outtime = 0;
	for(cnt=0;cnt<3;cnt++)
	{
		initDataPool(&ac6530rx);
		//AC6530_usart_write(&p_tmp,1);
		External_usart_write(&p_tmp,1);
		count_time_flag = 0;
		while(1)
		{
			//p_len = pool_recv_one_command(&ac6530rx,p_6530,10,AC6530_WAIT_TIME);
			p_len = pool_recv_one_command(&externalrx,p_6530,10,EXTERNAL_WAIT_TIME);
			if(p_len > 2)
			{
				Debug_usart_write(p_6530,p_len,INFO_DEBUG);
				break;
			}
			if(count_time_flag==1)
			{
				read_outtime++;
				if(read_outtime >= 3)
				{
					read_outtime = 0;
					return 0;
				}
				count_time_flag = 0;
			}
		}
		Debug_usart_write("read p over:",12,INFO_DEBUG);
		Debug_usart_write(p_6530,p_len,INFO_DEBUG);
		Debug_usart_write("\r\n",2,INFO_DEBUG);
		if(p_len > 0)
		{
			break;
		}
		if((cnt == 2) && (debug==0))
		{
			return 0;
		}
	}

	if(debug==0)
	{
		string_to_flodou(p,p_6530,p_len);
		string_to_flodou(v,v_6530,v_len);
		string_to_flodou(i,i_6530,i_len);
	}
	else
	{
		string_to_flodou(p,p_6530,5);
		string_to_flodou(v,v_6530,6);
		string_to_flodou(i,i_6530,3);
	}

	return 1;
}

#if 1
uint8_t Get_origin_pvi(uint8_t *data,uint32_t len,double *p,double *v,double *i)
{
	uint32_t tmp_p=0,tmp_v=0,tmp_i=0;
	uint32_t tmp = 0;

	uint32_t p_coe=0;
	uint32_t v_coe=0;
	uint32_t i_coe=0;
	uint32_t ret = 0;

	ret = Check_data_is_error(data,len);
	if(ret != 1)
	{
		return ret;
	}

	p_coe = (((data[14]<<8)|data[15])<<8)|data[16];
	v_coe = (((data[2]<<8)|data[3])<<8)|data[4];
	i_coe = (((data[8]<<8)|data[9])<<8)|data[10];

	tmp_p = (((data[17]<<8)|data[18])<<8)|data[19];
	tmp_v = (((tmp|data[5]<<8)|data[6])<<8)|data[7];
	tmp_i = (((data[11]<<8)|data[12])<<8)|data[13];

	*p = (double)(p_coe)/(tmp_p);
	*v = (double)(v_coe)/(tmp_v);
	*i = (double)(i_coe)/(tmp_i);

	return 1;
}
#endif


uint8_t Get_compute_pvi(uint8_t *data,uint32_t len,double *ret_p,double *ret_v,double *ret_i)
{
	double p=0,v=0,i=0;
	uint8_t ret = 0;

	ret = Get_origin_pvi(data,len,&p,&v,&i);
	if(ret==1)
	{
		*ret_p = p;
		*ret_v = v;
		*ret_i = i;
#if 1
		if(*ret_v < 200)
		{
			if(*ret_p > 6.f)
			{
				p *= 10;
				*ret_p = p*self_adjust_coea_ph_110+self_adjust_coeb_ph_110;
				*ret_p /= 10;
			}
			else
			{
				*ret_p = p*self_adjust_coea_pl_110+self_adjust_coeb_pl_110;
			}
			if(*ret_i > 1)
			{
				//*ret_i = i*self_adjust_coea_i_110+self_adjust_coeb_i_110;
				*ret_i = i*self_adjust_coea_il_110;
			}
			else
			{
				//*ret_i = i*self_adjust_coea_il_110+self_adjust_coeb_il_110;
				*ret_i = i*self_adjust_coea_il_110;
			}
			*ret_v = v + self_adjust_comv_110;
		}
		else
		{
			if(*ret_p > 25)
			{
				*ret_p = p*self_adjust_coea_ph+self_adjust_coeb_ph;
			}
			else
			{
				*ret_p = p*self_adjust_coea_pl+self_adjust_coeb_pl;
			}
			if(*ret_i > 2)
			{
				*ret_i = i*self_adjust_coea_i+self_adjust_coeb_i;
				//*ret_i = i*self_adjust_coea_il;
			}
			else
			{
				//*ret_i = i*self_adjust_coea_il+self_adjust_coeb_il;
				*ret_i = i*self_adjust_coea_il;
			}
			*ret_v = v + self_adjust_comv;
		}



#endif
		return 1;
	}

	return ret;
}


void Debug_write_coe110_info(void)
{
	uint8_t buf[10] = {0};
	double p_a = self_adjust_coea_pl_110;
	double p_b = self_adjust_coeb_pl_110;
	double p_r = self_adjust_coer_pl_110;

	double ph_a = self_adjust_coea_ph_110;
	double ph_b = self_adjust_coeb_ph_110;
	double ph_r = self_adjust_coer_ph_110;

	double il_a = self_adjust_coea_il_110;
	double il_b = self_adjust_coeb_il_110;
	double il_r = self_adjust_coer_il_110;

	double i_a = self_adjust_coea_i_110;
	double i_b = self_adjust_coeb_i_110;
	double i_r = self_adjust_coer_i_110;

	double c_v = self_adjust_comv_110;

	Debug_usart_write("PL_A_110:",9,INFO_DEBUG);
	if(p_a>0)
	{
		flodou_to_string(p_a,buf,Get_double_mantissa_len(&p_a),4);
	}
	else
	{
		p_a = -p_a;
		flodou_to_string(p_a,buf,Get_double_mantissa_len(&p_a),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("PL_B_110:",9,INFO_DEBUG);
	if(p_b>0)
	{
		flodou_to_string(p_b,buf,Get_double_mantissa_len(&p_b),4);
	}
	else
	{
		p_b = -p_b;
		flodou_to_string(p_b,buf,Get_double_mantissa_len(&p_b),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("PL_R_110:",9,INFO_DEBUG);
	if(p_r>0)
	{
		flodou_to_string(p_r,buf,Get_double_mantissa_len(&p_r),4);
	}
	else
	{
		p_r = -p_r;
		flodou_to_string(p_r,buf,Get_double_mantissa_len(&p_r),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n\r\n",4,INFO_DEBUG);
	//////////////////////////////////////////////////////////////////////////////////
	Debug_usart_write("PH_A_110:",9,INFO_DEBUG);
	if(p_a>0)
	{
		flodou_to_string(ph_a,buf,Get_double_mantissa_len(&ph_a),4);
	}
	else
	{
		p_a = -p_a;
		flodou_to_string(ph_a,buf,Get_double_mantissa_len(&ph_a),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("PH_B_110:",9,INFO_DEBUG);
	if(p_b>0)
	{
		flodou_to_string(ph_b,buf,Get_double_mantissa_len(&ph_b),4);
	}
	else
	{
		p_b = -p_b;
		flodou_to_string(ph_b,buf,Get_double_mantissa_len(&ph_b),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("PH_R_110:",9,INFO_DEBUG);
	if(p_r>0)
	{
		flodou_to_string(ph_r,buf,Get_double_mantissa_len(&ph_r),4);
	}
	else
	{
		p_r = -p_r;
		flodou_to_string(ph_r,buf,Get_double_mantissa_len(&ph_r),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n\r\n",4,INFO_DEBUG);
	//////////////////////////////////////////////////////////////////////////////////
	Debug_usart_write("IL_A_110:",9,INFO_DEBUG);
	if(il_a>0)
	{
		flodou_to_string(il_a,buf,Get_double_mantissa_len(&il_a),4);
	}
	else
	{
		il_a = -il_a;
		flodou_to_string(il_a,buf,Get_double_mantissa_len(&il_a),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("IL_B_110:",9,INFO_DEBUG);
	if(il_b>0)
	{
		flodou_to_string(il_b,buf,Get_double_mantissa_len(&il_b),4);
	}
	else
	{
		il_b = -il_b;
		flodou_to_string(il_b,buf,Get_double_mantissa_len(&il_b),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("IL_R_110:",9,INFO_DEBUG);
	if(il_r>0)
	{
		flodou_to_string(i_r,buf,Get_double_mantissa_len(&il_r),4);
	}
	else
	{
		il_r = -il_r;
		flodou_to_string(il_r,buf,Get_double_mantissa_len(&il_r),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n\r\n",4,INFO_DEBUG);

	//////////////////////////////////////////////////////////////////////////////////
	Debug_usart_write("I_A_110:",8,INFO_DEBUG);
	if(i_a>0)
	{
		flodou_to_string(i_a,buf,Get_double_mantissa_len(&i_a),4);
	}
	else
	{
		i_a = -i_a;
		flodou_to_string(i_a,buf,Get_double_mantissa_len(&i_a),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("I_B_110:",8,INFO_DEBUG);
	if(i_b>0)
	{
		flodou_to_string(i_b,buf,Get_double_mantissa_len(&i_b),4);
	}
	else
	{
		i_b = -i_b;
		flodou_to_string(i_b,buf,Get_double_mantissa_len(&i_b),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("I_R_110:",8,INFO_DEBUG);
	if(i_r>0)
	{
		flodou_to_string(i_r,buf,Get_double_mantissa_len(&i_r),4);
	}
	else
	{
		i_r = -i_r;
		flodou_to_string(i_r,buf,Get_double_mantissa_len(&i_r),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("C_V_110:",8,INFO_DEBUG);
	if(c_v>0)
	{
		flodou_to_string(c_v,buf,Get_double_mantissa_len(&c_v),4);
	}
	else
	{
		c_v = -c_v;
		flodou_to_string(c_v,buf,Get_double_mantissa_len(&c_v),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);
}

void Debug_write_coe_info(void)
{
	uint8_t buf[10] = {0};
	double p_a = self_adjust_coea_pl;
	double p_b = self_adjust_coeb_pl;
	double p_r = self_adjust_coer_pl;

	double ph_a = self_adjust_coea_ph;
	double ph_b = self_adjust_coeb_ph;
	double ph_r = self_adjust_coer_ph;

	double il_a = self_adjust_coea_il;
	double il_b = self_adjust_coeb_il;
	double il_r = self_adjust_coer_il;

	double i_a = self_adjust_coea_i;
	double i_b = self_adjust_coeb_i;
	double i_r = self_adjust_coer_i;

	double c_v = self_adjust_comv;
	//double cl_v = self_adjust_comv;

	Debug_usart_write("PL_A:",5,INFO_DEBUG);
	if(p_a>0)
	{
		flodou_to_string(p_a,buf,Get_double_mantissa_len(&p_a),4);
	}
	else
	{
		p_a = -p_a;
		flodou_to_string(p_a,buf,Get_double_mantissa_len(&p_a),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("PL_B:",5,INFO_DEBUG);
	if(p_b>0)
	{
		flodou_to_string(p_b,buf,Get_double_mantissa_len(&p_b),4);
	}
	else
	{
		p_b = -p_b;
		flodou_to_string(p_b,buf,Get_double_mantissa_len(&p_b),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("PL_R:",5,INFO_DEBUG);
	if(p_r>0)
	{
		flodou_to_string(p_r,buf,Get_double_mantissa_len(&p_r),4);
	}
	else
	{
		p_r = -p_r;
		flodou_to_string(p_r,buf,Get_double_mantissa_len(&p_r),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n\r\n",4,INFO_DEBUG);
	//////////////////////////////////////////////////////////////////////////////////
	Debug_usart_write("PH_A:",5,INFO_DEBUG);
	if(p_a>0)
	{
		flodou_to_string(ph_a,buf,Get_double_mantissa_len(&ph_a),4);
	}
	else
	{
		p_a = -p_a;
		flodou_to_string(ph_a,buf,Get_double_mantissa_len(&ph_a),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("PH_B:",5,INFO_DEBUG);
	if(p_b>0)
	{
		flodou_to_string(ph_b,buf,Get_double_mantissa_len(&ph_b),4);
	}
	else
	{
		p_b = -p_b;
		flodou_to_string(ph_b,buf,Get_double_mantissa_len(&ph_b),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("PH_R:",5,INFO_DEBUG);
	if(p_r>0)
	{
		flodou_to_string(ph_r,buf,Get_double_mantissa_len(&ph_r),4);
	}
	else
	{
		p_r = -p_r;
		flodou_to_string(ph_r,buf,Get_double_mantissa_len(&ph_r),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n\r\n",4,INFO_DEBUG);
	//////////////////////////////////////////////////////////////////////////////////
	Debug_usart_write("IL_A:",5,INFO_DEBUG);
	if(il_a>0)
	{
		flodou_to_string(il_a,buf,Get_double_mantissa_len(&il_a),4);
	}
	else
	{
		il_a = -il_a;
		flodou_to_string(il_a,buf,Get_double_mantissa_len(&il_a),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("IL_B:",5,INFO_DEBUG);
	if(il_b>0)
	{
		flodou_to_string(il_b,buf,Get_double_mantissa_len(&il_b),4);
	}
	else
	{
		il_b = -il_b;
		flodou_to_string(il_b,buf,Get_double_mantissa_len(&il_b),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("IL_R:",5,INFO_DEBUG);
	if(il_r>0)
	{
		flodou_to_string(il_r,buf,Get_double_mantissa_len(&il_r),4);
	}
	else
	{
		il_r = -il_r;
		flodou_to_string(il_r,buf,Get_double_mantissa_len(&il_r),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n\r\n",4,INFO_DEBUG);
	//////////////////////////////////////////////////////////////////////////////////
	Debug_usart_write("I_A:",4,INFO_DEBUG);
	if(i_a>0)
	{
		flodou_to_string(i_a,buf,Get_double_mantissa_len(&i_a),4);
	}
	else
	{
		i_a = -i_a;
		flodou_to_string(il_a,buf,Get_double_mantissa_len(&i_a),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("I_B:",4,INFO_DEBUG);
	if(i_b>0)
	{
		flodou_to_string(i_b,buf,Get_double_mantissa_len(&i_b),4);
	}
	else
	{
		i_b = -i_b;
		flodou_to_string(i_b,buf,Get_double_mantissa_len(&i_b),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("I_R:",4,INFO_DEBUG);
	if(i_r>0)
	{
		flodou_to_string(i_r,buf,Get_double_mantissa_len(&i_r),4);
	}
	else
	{
		i_r = -i_r;
		flodou_to_string(i_r,buf,Get_double_mantissa_len(&i_r),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("C_V:",4,INFO_DEBUG);
	if(c_v>0)
	{
		flodou_to_string(c_v,buf,Get_double_mantissa_len(&c_v),4);
	}
	else
	{
		c_v = -c_v;
		flodou_to_string(c_v,buf,Get_double_mantissa_len(&c_v),4);
		Debug_usart_write("-",1,INFO_DEBUG);
	}
	Debug_usart_write(buf,6,INFO_DEBUG);
	Debug_usart_write("\r\n\r\n",4,INFO_DEBUG);

}

void key_operate(void)
{
	uint8_t c = 0;

	if(key_status != key_backup)   //power on-off
	{
	    if(key_status == 0)
	    {
	    	if(RELAY_CONTR_KEY_READ==0)
	    	{
		    	if(relay_sta==1)
		    		GPIO_SetBits(GPIOC,GPIO_Pin_0);
		    	else
		    		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
		    	Debug_write_coe_info();
		    	Debug_write_coe110_info();
		    	relay_sta = ~relay_sta;
	    	}
	    	if(SELF_ADJUST_KEY_READ==0)
	    	{
#if 0
	    		if(operate_mode != 0)
	    			operate_mode = 0;
	    		else
	    			operate_mode = SELF_ADJUST_MODE;
#endif
	    		relay_test();
	    	}
	    	if(EXTERNAL_KEY_READ==0)
	    	{
#if 1
	    		if(operate_mode != 0)
	    			operate_mode = 0;
	    		else
	    			operate_mode = EXTERNAL_ADJUST_MODE;
#endif
//	    		relay_test();
	    	}
	    }
		if(operate_mode==0 && RELAY_CONTR_KEY_READ)
		{
			iic_2864_clear_one(KEY_CHANGE);
			lcd_show_pvi_info(0,0,0);
		}
	    key_backup = key_status;
		c = operate_mode + '0';
		Debug_usart_write("mode:",5,INFO_DEBUG);
		Debug_usart_write(&c,1,INFO_DEBUG);
		Debug_usart_write("\r\n",2,INFO_DEBUG);
	}
	initDataPool(&cse7766rx);
}

void change_load_size(uint8_t *ch_sta,uint32_t load_size)
{
#if 1
	if(load_size==1)
			ch_sta[0] = 1;
		else if(load_size==3)
		{
			ch_sta[1] = 1;;
		}
		else if(load_size==4)
		{
			ch_sta[0] = 1;ch_sta[1] = 1;
		}
		else if(load_size==6)
		{
			ch_sta[0] = 1;ch_sta[2] = 1;
		}
		else if(load_size==8)
		{
			ch_sta[1] = 1;ch_sta[2] = 1;
		}
		else if(load_size==10)
		{
			ch_sta[3] = 1;
		}
		else if(load_size==11)
		{
			ch_sta[0] = 1;ch_sta[3] = 1;
		}
		else if(load_size==13)
		{
			ch_sta[1] = 1;ch_sta[3] = 1;
		}
		else if(load_size==14)
		{
			ch_sta[0] = 1;ch_sta[1] = 1;ch_sta[3] = 1;
		}
		else if(load_size==15)
		{
			ch_sta[3] = 1;ch_sta[2] = 1;
		}
		else if(load_size==16)
		{
			ch_sta[0] = 1;ch_sta[2] = 1;ch_sta[3] = 1;
		}
		else if(load_size==18)
		{
			ch_sta[1] = 1;ch_sta[2] = 1;ch_sta[3] = 1;
		}
		else if(load_size==20)
		{
			ch_sta[3] = 1;ch_sta[4] = 1;
		}
		else if(load_size==21)
		{
			ch_sta[0] = 1;ch_sta[3] = 1;ch_sta[4] = 1;
		}
		else if(load_size==23)
		{
			ch_sta[1] = 1;ch_sta[3] = 1;ch_sta[4] = 1;
		}
		else if(load_size==25)
		{
			ch_sta[3] = 1;ch_sta[2] = 1;ch_sta[4] = 1;
		}
		else if(load_size==26)
		{
			ch_sta[0] = 1;ch_sta[2] = 1;ch_sta[3] = 1;ch_sta[4] = 1;
		}
		else if(load_size==28)
		{
			ch_sta[1] = 1;ch_sta[2] = 1;ch_sta[3] = 1;ch_sta[4] = 1;
		}
		else if(load_size==30)
		{
			ch_sta[5] = 1;ch_sta[3] = 1;ch_sta[4] = 1;
		}
		else if(load_size==31)
		{
			ch_sta[0] = 1;ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[5] = 1;
		}
		else if(load_size==33)
		{
			ch_sta[1] = 1;ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[5] = 1;
		}
		else if(load_size==34)
		{
			ch_sta[0] = 1;ch_sta[1] = 1;ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[5] = 1;
		}
		else if(load_size==36)
		{
			ch_sta[0] = 1;ch_sta[2] = 1;ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[5] = 1;
		}
		else if(load_size==38)
		{
			ch_sta[1] = 1;ch_sta[2] = 1;ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[5] = 1;
		}
		else if(load_size==39)
		{
			ch_sta[0] = 1;ch_sta[1] = 1;ch_sta[2] = 1;ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[5] = 1;
		}
		else if(load_size==50)
		{
			ch_sta[6] = 1;
		}
		else if(load_size==60)
		{
			ch_sta[3] = 1;ch_sta[6] = 1;
		}
		else if(load_size==70)
		{
			ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[6] = 1;
		}
		else if(load_size==80)
		{
			ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[5] = 1;ch_sta[6] = 1;
		}
		else if(load_size==100)
		{
			ch_sta[6] = 1;ch_sta[7] = 1;
		}
		else if(load_size==130)
		{
			ch_sta[5] = 1;ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[6] = 1;ch_sta[7] = 1;
		}
		else if(load_size==139)
		{
			ch_sta[0] = 1;ch_sta[1] = 1;ch_sta[2] = 1;ch_sta[3] = 1;ch_sta[4] = 1;ch_sta[5] = 1;ch_sta[6] = 1;ch_sta[7] = 1;
		}
		else if(load_size==500)
		{
			ch_sta[8] = 1;
		}
		else if(load_size==1000)
		{
			ch_sta[8] = 1;ch_sta[9] = 1;
		}
		else if(load_size==1500)
		{
			ch_sta[8] = 1;ch_sta[9] = 1;ch_sta[10] = 1;
		}
		else if(load_size==2000)
		{
			ch_sta[8] = 1;ch_sta[9] = 1;ch_sta[10] = 1;ch_sta[11] = 1;
		}
#endif

}

void operate_ch_relay(uint8_t *ch)
{
	uint8_t i = 0;

	for(i=0;i<16;i++)
	{
		if(ch[i]==0)
		{
			GPIO_ResetBits(ch_io[i],ch_pin[i]);
		}
		else
		{
			GPIO_SetBits(ch_io[i],ch_pin[i]);
		}
	}
}

uint8_t  Check_c76_return_isok(uint8_t *data,uint32_t len)
{
	uint8_t ret = 0;
	uint8_t i = 0;

	if(len < 10)
	{
		return 0;
	}

	for(i=0;i<len;i++)
	{
		if(data[i]=='K')
		{
			break;
		}
		if(data[i]=='\r'||data[i]=='\n'||data[i]==',')
		{
			continue;
		}
		else if(data[i]=='O'||data[i]=='K')
		{
			continue;
		}
		else if(data[i]>='0'&& data[i]<='9')
		{
			continue;
		}
		else
		{
			return 0;
		}
	}

	return 1;
}

uint32_t Get_computer_external_pvi(uint8_t *data,uint32_t len,double *p,double *v,double *i)
{
	uint8_t tmp = 0;
	uint8_t c = 0;
	uint8_t ok_cnt = 0;
	uint8_t m = 0,n = 0;;
	uint8_t y = 0;
	uint8_t ret = 0;
	uint8_t p_buf[10] = {0};
	uint8_t v_buf[10] = {0};
	uint8_t i_buf[10] = {0};
	double p_ret = 0,v_ret = 0,i_ret = 0;

	Debug_usart_write("Go get computer external pvi\r\n",30,INFO_DEBUG);
	Debug_usart_write("c76 data:",9,INFO_DEBUG);
	Debug_usart_write(data,len,INFO_DEBUG);
	Debug_usart_write("start computer\r\n",16,INFO_DEBUG);
	ret = Check_c76_return_isok(data,len);
	if(ret==0)
	{
		return 0;
	}

	while(1)
	{
		if(data[y] >= '0' && data[y] <= '9')
		{
			break;
		}
		else
		{
			y++;
		}
	}

	for(tmp=y;tmp<len;tmp++)
	{
		if(data[tmp] == 'K')
		{
			ok_cnt++;
			if(ok_cnt == 1)
			{
				break;
			}
		}

		if(data[tmp] == ',')
		{
			m++;
			n = 0;
			continue;
		}
		if(m==0)
		{
			p_buf[n] = data[tmp];
			n++;
		}
		if(m==1)
		{
			i_buf[n] = data[tmp];
			n++;
		}
		if(m==2)
		{
			v_buf[n] = data[tmp];
			n++;
		}
	}
	Debug_usart_write("stop computer\r\n",16,INFO_DEBUG);
	Debug_usart_write("start string to float\r\n",23,INFO_DEBUG);
	Debug_usart_write(p_buf,str_len(p_buf),INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);
	string_to_flodou(&p_ret,p_buf,str_len(p_buf));

	Debug_usart_write(v_buf,str_len(v_buf),INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);
	string_to_flodou(&v_ret,v_buf,str_len(v_buf)-3);

	Debug_usart_write(i_buf,str_len(i_buf),INFO_DEBUG);
	Debug_usart_write("\r\n",2,INFO_DEBUG);
	string_to_flodou(&i_ret,i_buf,str_len(i_buf));

	Debug_usart_write("stop string to float\r\n",22,INFO_DEBUG);

	*p = p_ret/1000;
	*v = v_ret/1000;
	*i = i_ret/1000;

	Debug_usart_write("out get computer external pvi\r\n",31,INFO_DEBUG);
	return 1;
}

void Send_result_to_c76(uint8_t result,double pl_a,double pl_b,double il_a,double il_b,double vl_coe,
						double p_a,double p_b,double i_a,double i_b,double v_coe)
{
	uint8_t send_data[100] = {"AT+C76END=0,"};
	uint8_t p_a_buf[10] = {0};
	uint8_t p_b_buf[10] = {0};
	uint8_t i_a_buf[10] = {0};
	uint8_t i_b_buf[10] = {0};
	uint8_t v_coe_buf[10] = {0};

	uint8_t ph_a_buf[10] = {0};
	uint8_t ph_b_buf[10] = {0};
	uint8_t ih_a_buf[10] = {0};
	uint8_t ih_b_buf[10] = {0};
	uint8_t vh_coe_buf[10] = {0};

	uint8_t test_buf[10] = {0};

	double p_a_tmp = pl_a;
	double p_b_tmp = pl_b;
	double i_a_tmp = il_a;
	double i_b_tmp = il_b;
	double v_coe_tmp = vl_coe;

	double ph_a_tmp = p_a;
	double ph_b_tmp = p_b;
	double ih_a_tmp = i_a;
	double ih_b_tmp = i_b;
	double vh_coe_tmp = v_coe;

	if(result==1)
	{
		send_data[10] = '1';
	}
	else
	{
		send_data[10] = '0';
	}

	if(p_a_tmp<0)
			p_a_tmp = -pl_a;
	if(p_b_tmp<0)
			p_b_tmp = -pl_b;
	if(i_a_tmp<0)
			i_a_tmp = -il_a;
	if(i_b_tmp<0)
			i_b_tmp = -il_b;
	if(v_coe_tmp<0)
			v_coe_tmp = -vl_coe;
	if(ph_a_tmp<0)
			ph_a_tmp = -p_a;
	if(ph_b_tmp<0)
			ph_b_tmp = -p_b;
	if(ih_a_tmp<0)
			ih_a_tmp = -i_a;
	if(ih_b_tmp<0)
			ih_b_tmp = -i_b;
	if(vh_coe_tmp<0)
			vh_coe_tmp = -v_coe;
#if 1
	if(pl_a < 0)
	{
		Debug_usart_write("-",1,TEST_DEBUG);
	}
	flodou_to_string(p_a_tmp,test_buf,Get_double_mantissa_len(&p_a_tmp),3);
	Debug_usart_write(test_buf,9,TEST_DEBUG);
	Debug_usart_write("\r\n",2,TEST_DEBUG);

	if(pl_b < 0)
	{
			Debug_usart_write("-",1,TEST_DEBUG);
	}
	flodou_to_string(p_b_tmp,test_buf,Get_double_mantissa_len(&p_b_tmp),3);
	Debug_usart_write(test_buf,9,TEST_DEBUG);
	Debug_usart_write("\r\n",2,TEST_DEBUG);

	if(il_a < 0)
	{
		Debug_usart_write("-",1,TEST_DEBUG);
	}
	flodou_to_string(i_a_tmp,test_buf,Get_double_mantissa_len(&i_a_tmp),3);
	Debug_usart_write(test_buf,9,TEST_DEBUG);
	Debug_usart_write("\r\n",2,TEST_DEBUG);

	if(il_b < 0)
	{
		Debug_usart_write("-",1,TEST_DEBUG);
	}
	flodou_to_string(i_b_tmp,test_buf,Get_double_mantissa_len(&i_b_tmp),3);
	Debug_usart_write(test_buf,9,TEST_DEBUG);
	Debug_usart_write("\r\n",2,TEST_DEBUG);

	if(p_a < 0)
	{
		Debug_usart_write("-",1,TEST_DEBUG);
	}
	flodou_to_string(ph_a_tmp,test_buf,Get_double_mantissa_len(&ph_a_tmp),3);
	Debug_usart_write(test_buf,9,TEST_DEBUG);
	Debug_usart_write("\r\n",2,TEST_DEBUG);

	if(p_b < 0)
	{
			Debug_usart_write("-",1,TEST_DEBUG);
	}
	flodou_to_string(ph_b_tmp,test_buf,Get_double_mantissa_len(&ph_b_tmp),3);
	Debug_usart_write(test_buf,9,TEST_DEBUG);
	Debug_usart_write("\r\n",2,TEST_DEBUG);

	if(i_a < 0)
	{
		Debug_usart_write("-",1,TEST_DEBUG);
	}
	flodou_to_string(ih_a_tmp,test_buf,Get_double_mantissa_len(&ih_a_tmp),3);
	Debug_usart_write(test_buf,9,TEST_DEBUG);
	Debug_usart_write("\r\n",2,TEST_DEBUG);

	if(i_b < 0)
	{
		Debug_usart_write("-",1,TEST_DEBUG);
	}
	flodou_to_string(ih_b_tmp,test_buf,Get_double_mantissa_len(&ih_b_tmp),3);
	Debug_usart_write(test_buf,9,TEST_DEBUG);
	Debug_usart_write("\r\n",2,TEST_DEBUG);
#endif
	p_a_tmp *= 1000;
	p_b_tmp *= 1000;
	i_a_tmp *= 1000;
	i_b_tmp *= 1000;
	v_coe_tmp *= 1000;

	ph_a_tmp *= 1000;
	ph_b_tmp *= 1000;
	ih_a_tmp *= 1000;
	ih_b_tmp *= 1000;
	vh_coe_tmp *= 1000;

	if(((uint32_t)p_a_tmp)%10 > 5)
		p_a_tmp += 10;
	if(((uint32_t)p_b_tmp)%10 > 5)
		p_b_tmp += 10;
	if(((uint32_t)i_a_tmp)%10 > 5)
		i_a_tmp += 10;
	if(((uint32_t)i_b_tmp)%10 > 5)
		i_b_tmp += 10;
	if(((uint32_t)v_coe_tmp)%10 > 5)
		v_coe_tmp += 10;

	if(((uint32_t)ph_a_tmp)%10 > 5)
		ph_a_tmp += 10;
	if(((uint32_t)ph_b_tmp)%10 > 5)
		ph_b_tmp += 10;
	if(((uint32_t)ih_a_tmp)%10 > 5)
		ih_a_tmp += 10;
	if(((uint32_t)ih_b_tmp)%10 > 5)
		ih_b_tmp += 10;
	if(((uint32_t)vh_coe_tmp)%10 > 5)
		vh_coe_tmp += 10;

	p_a_tmp /= 10;
	p_b_tmp /= 10;
	i_a_tmp /= 10;
	i_b_tmp /= 10;
	v_coe_tmp /= 10;

	ph_a_tmp /= 10;
	ph_b_tmp /= 10;
	ih_a_tmp /= 10;
	ih_b_tmp /= 10;
	vh_coe_tmp /= 10;

	//Itoa((uint32_t)p_a_tmp,p_a_buf);
	//Itoa((uint32_t)p_b_tmp,p_b_buf);
	//Itoa((uint32_t)i_a_tmp,i_a_buf);
	//Itoa((uint32_t)i_b_tmp,i_b_buf);

	flodou_to_string(p_a_tmp,p_a_buf,Get_double_mantissa_len(&p_a_tmp),0);
	flodou_to_string(p_b_tmp,p_b_buf,Get_double_mantissa_len(&p_b_tmp),0);
	flodou_to_string(i_a_tmp,i_a_buf,Get_double_mantissa_len(&i_a_tmp),0);
	flodou_to_string(i_b_tmp,i_b_buf,Get_double_mantissa_len(&i_b_tmp),0);
	flodou_to_string(v_coe_tmp,v_coe_buf,Get_double_mantissa_len(&v_coe_tmp),0);

	flodou_to_string(ph_a_tmp,ph_a_buf,Get_double_mantissa_len(&ph_a_tmp),0);
	flodou_to_string(ph_b_tmp,ph_b_buf,Get_double_mantissa_len(&ph_b_tmp),0);
	flodou_to_string(ih_a_tmp,ih_a_buf,Get_double_mantissa_len(&ih_a_tmp),0);
	flodou_to_string(ih_b_tmp,ih_b_buf,Get_double_mantissa_len(&ih_b_tmp),0);
	flodou_to_string(vh_coe_tmp,vh_coe_buf,Get_double_mantissa_len(&vh_coe_tmp),0);

	if(pl_a < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),p_a_buf,str_len(p_a_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(pl_b < 0)
	{
			str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),p_b_buf,str_len(p_b_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(il_a < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),i_a_buf,str_len(i_a_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(il_b < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),i_b_buf,str_len(i_b_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(vl_coe < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),v_coe_buf,str_len(v_coe_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(p_a < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),ph_a_buf,str_len(ph_a_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(p_b < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),ph_b_buf,str_len(ph_b_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(i_a < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),ih_a_buf,str_len(ih_a_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(i_b < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),ih_b_buf,str_len(ih_b_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);

	if(v_coe < 0)
	{
		str_cat(send_data,str_len(send_data),(uint8_t *)"-",(uint32_t)1);
	}
	str_cat(send_data,str_len(send_data),vh_coe_buf,str_len(vh_coe_buf));


	if(result==1)
	{
		External_usart_write(send_data,str_len(send_data));
		External_usart_write((uint8_t *)"\r\n",2);
	}

	Debug_usart_write(send_data,str_len(send_data),'Y');
	Debug_usart_write("\r\n",2,'Y');
}

int32_t Go_self_adjust(void)
{
	uint8_t ch_sta[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//1 2 3 4 5 6 7 8 9 10 60 60 600 600 600 600 600 600
	//uint32_t load_size[28] = {1,2,3,4,5,6,7,8,9,10,15,60,120,600,660,720,1200,1260,1320,1800,1860,1920,2400,2460,2520,3000,3060,3160};
	//uint32_t load_size[16] = {1,3,6,12,20,50,70,100,170,500,600,1000,1500,2000,2500,3000};
	//uint32_t load_size[12] = {3,12,20,50,70,100,170,500,600,1000,1500,2000};
	uint32_t load_size[35] = {1,3,4,6,8,10,11,13,14,15,16,18,20,21,23,25,26,28,30,33,34,36,38,
								  39,50,60,70,80,100,130,139,500,1000,1500,2000};
	uint32_t load_cnt = 35;
	uint8_t i = 0,j = 0,z = 0,k = 0;
	uint8_t p_cnt = 0,v_cnt = 0,i_cnt = 0;
	uint32_t m = 0;
	uint8_t ret = 0;
	uint8_t data[240] = {0};
	uint8_t cnt_buf[2] = {0};
	uint8_t c = 0;

	uint8_t test_buf[10] = {0};
	uint8_t send_data[150] = {0};
	uint8_t read_count = 0;

	double coea_pl = 0;
	double coeb_pl = 0;
	double coer_pl = 0;

	double coea_ph = 0;
	double coeb_ph = 0;
	double coer_ph = 0;

	double coea_i = 0;
	double coeb_i = 0;
	double coer_i = 0;

	double coea_il = 0;
	double coeb_il = 0;
	double coer_il = 0;

	double p_7766[36] = {0};
	double v_7766[36] = {0};
	double i_7766[36] = {0};

	double pp_7766[36] = {0};
	double vv_7766[36] = {0};
	double ii_7766[36] = {0};

	double p_6530[36] = {0};
	double v_6530[36] = {0};
	double i_6530[36] = {0};

	double pp_6530[36] = {0};
	double vv_6530[36] = {0};
	double ii_6530[36] = {0};

	double p_tmp_6530[10] = {0};
	double v_tmp_6530[10] = {0};
	double i_tmp_6530[10] = {0};

	double p_tmp_7766[10] = {0};
	double v_tmp_7766[10] = {0};
	double i_tmp_7766[10] = {0};

	double ac6530_com_v = 0;
	double self_com_v = 0;

	for(i=0;i<load_cnt;i++)
	{
		change_load_size(ch_sta,load_size[i]);
		operate_ch_relay(ch_sta);
		memset(ch_sta,0x00, sizeof(ch_sta));
		memset(p_tmp_6530,0x00, sizeof(p_tmp_6530));
		memset(v_tmp_6530,0x00, sizeof(v_tmp_6530));
		memset(i_tmp_6530,0x00, sizeof(i_tmp_6530));
		memset(p_tmp_7766,0x00, sizeof(p_tmp_7766));
		memset(v_tmp_7766,0x00, sizeof(v_tmp_7766));
		memset(i_tmp_7766,0x00, sizeof(i_tmp_7766));
		initDataPool(&cse7766rx);
		while(1)
		{
			if(count_time_flag==1)
			{
				read_count++;
				initDataPool(&cse7766rx);
				if(read_count >= 4)
				{
					read_count = 0;
					break;
				}
				count_time_flag = 0;
			}
		}
		initDataPool(&cse7766rx);
		z = 0;
		k = 0;
		for(j=0;j<10;j++)
		{
			if(load_size[i] < 50)
			{
				if(Get_6530_pvi(&p_tmp_6530[j],&v_tmp_6530[j],&i_tmp_6530[j])==1)
				{
					k++;
				}
				else
				{
					p_tmp_6530[j] = 0;
					v_tmp_6530[j] = 0;
					i_tmp_6530[j] = 0;
				}
				Debug_usart_write("read 6530 over\r\n",16,INFO_DEBUG);
			}
			else
			{
				if(j%4==0)
				{
					if(Get_6530_pvi(&p_tmp_6530[j/4],&v_tmp_6530[j/4],&i_tmp_6530[j/4])==1)
					{
						k++;
					}
					else
					{
						p_tmp_6530[j/4] = 0;
						v_tmp_6530[j/4] = 0;
						i_tmp_6530[j/4] = 0;
					}
					Debug_usart_write("read 6530 over\r\n",16,INFO_DEBUG);
				}
			}
			if(load_size[i]>0)
			{
				count_time_flag = 0;
				while(1)
				{
					if(count_time_flag==1)
					{
						read_count++;
						if(read_count >= 10)
						{
							//memset(ch_sta,0x00, sizeof(ch_sta));
							//operate_ch_relay(ch_sta);
							//return -3;
							break;
						}
						count_time_flag = 0;
					}
					pool_recv_one_command(&cse7766rx,&data[24*j],24,CSE_7766_POOL);
					Debug_usart_write(&data[24*j],24,CSE7766_DEBUG);
					ret = Check_data_is_error(&data[24*j],24);
					if(ret==1)
					{
						break;
					}
					else
					{
						memset(&data[24*j],0x00,24);
					}
				}
			}
			else
			{
				pool_recv_one_command(&cse7766rx,&data[24*j],24,CSE_7766_POOL);
			}

		}

		if(k==0)
		{
			memset(ch_sta,0x00, sizeof(ch_sta));
			operate_ch_relay(ch_sta);
			return -1;
		}
		//Debug_usart_write("load_cnt:",9,INFO_DEBUG);
		//cnt_buf[0] = (i/10+'0');
		//cnt_buf[1] = (i%10+'0');
		//Debug_usart_write(cnt_buf,2,INFO_DEBUG);
		//Debug_usart_write("\r\n",2,INFO_DEBUG);

		for(j=0;j<k;j++)
		{
			p_6530[i] += p_tmp_6530[j];
			v_6530[i] += v_tmp_6530[j];
			i_6530[i] += i_tmp_6530[j];
		}


		flodou_to_string(p_6530[i],test_buf,5,4);
		Debug_usart_write(test_buf,9,TEST_DEBUG);
		Debug_usart_write("\r\n",2,TEST_DEBUG);

		p_6530[i] /= k;
		v_6530[i] /= k;
		i_6530[i] /= k;

		flodou_to_string(p_6530[i],test_buf,5,4);
#if 1
		Debug_usart_write(test_buf,9,TEST_DEBUG);
		Debug_usart_write("\r\n",2,TEST_DEBUG);


#endif

		for(j=0;j<10;j++)
		{
			if(Get_origin_pvi(&data[24*j],24,&p_tmp_7766[j],&v_tmp_7766[j],&i_tmp_7766[j])==1)
			{
				z++;
			}
			else
			{
				p_tmp_7766[j] = 0;
				v_tmp_7766[j] = 0;
				i_tmp_7766[j] = 0;
			}
		}
		//if(z==0)
		//{
			//memset(ch_sta,0x00, sizeof(ch_sta));
			//operate_ch_relay(ch_sta);
			//return -2;
		//}
		for(j=0;j<10;j++)
		{
			p_7766[i] += p_tmp_7766[j];
			v_7766[i] += v_tmp_7766[j];
			i_7766[i] += i_tmp_7766[j];
		}

		flodou_to_string(p_7766[i],test_buf,5,4);
		Debug_usart_write(test_buf,9,TEST_DEBUG);
		Debug_usart_write("\r\n",2,TEST_DEBUG);

		p_7766[i] /= z;
		v_7766[i] /= z;
		i_7766[i] /= z;

		flodou_to_string(p_7766[i],test_buf,5,4);
		Debug_usart_write(test_buf,10,TEST_DEBUG);
		Debug_usart_write("\r\n\r\n",4,TEST_DEBUG);
#if 1
		flodou_to_string(p_6530[i],test_buf,5,4);
		if(i==0)
		{
			str_cat(send_data,str_len(send_data),"\r\n",2);
		}
		str_cat(send_data,str_len(send_data),test_buf,str_len(test_buf));
		str_cat(send_data,str_len(send_data),"\t",1);
		flodou_to_string(v_6530[i],test_buf,5,4);
		str_cat(send_data,str_len(send_data),test_buf,str_len(test_buf));
		str_cat(send_data,str_len(send_data),"\t",1);
		flodou_to_string(i_6530[i],test_buf,5,4);
		str_cat(send_data,str_len(send_data),test_buf,str_len(test_buf));
		str_cat(send_data,str_len(send_data),"\t",1);

		flodou_to_string(p_7766[i],test_buf,5,4);
		str_cat(send_data,str_len(send_data),test_buf,str_len(test_buf));
		str_cat(send_data,str_len(send_data),"\t",1);
		flodou_to_string(v_7766[i],test_buf,5,4);
		str_cat(send_data,str_len(send_data),test_buf,str_len(test_buf));
		str_cat(send_data,str_len(send_data),"\t",1);
		flodou_to_string(i_7766[i],test_buf,5,4);
		str_cat(send_data,str_len(send_data),test_buf,str_len(test_buf));
		str_cat(send_data,str_len(send_data),"\r\n",2);

		External_usart_write(send_data,str_len(send_data));
		memset(send_data,0x00, sizeof(send_data));
#endif
		if(3*i < 100)
		{
			lcd_change_percent_info(i*3);
		}
	}
	//c = i + '0';
	//Debug_usart_write(&c,1,INFO_DEBUG);
	lcd_change_percent_info(100);

	k = 0;
	for(j=0;j<load_cnt;j++)
	{
		if((v_6530[j] > 0.0001) && (v_7766[j] > 0.0001))
		{
			if(v_7766[j] > (v_6530[j]*0.7) && v_7766[j] < (v_6530[j]*1.3))
			{
				ac6530_com_v += v_6530[j];
				self_com_v += v_7766[j];
				k++;
			}
		}
	}

	ac6530_com_v /= k;
	self_com_v /= k;

	for(m=0;m<load_cnt;m++)
	{
		if(p_6530[m] > 0.0001 && p_7766[m] > 0.0001)
		{
			if((p_7766[m] > (p_6530[m]*0.7)) && (p_7766[m] < (p_6530[m]*1.3)))
			{
				pp_6530[p_cnt] = p_6530[m];
				pp_7766[p_cnt] = p_7766[m];
				p_cnt++;

				flodou_to_string(pp_6530[p_cnt],test_buf,5,4);
				Debug_usart_write(test_buf,10,TEST_DEBUG);
				Debug_usart_write("\t",1,TEST_DEBUG);

				flodou_to_string(pp_7766[p_cnt],test_buf,5,4);
				Debug_usart_write(test_buf,10,TEST_DEBUG);
				Debug_usart_write("\t",1,TEST_DEBUG);
			}
		}
		if(i_6530[m] > 0.0001 && i_7766[m] > 0.0001)
		{
			if((i_7766[m] > (i_6530[m]*0.7)) && (i_7766[m] < (i_6530[m]*1.3)))
			{
				ii_6530[i_cnt] = i_6530[m];
				ii_7766[i_cnt] = i_7766[m];
				i_cnt++;

				flodou_to_string(ii_6530[i_cnt],test_buf,5,4);
				Debug_usart_write(test_buf,10,TEST_DEBUG);
				Debug_usart_write("\t",1,TEST_DEBUG);

				flodou_to_string(ii_7766[i_cnt],test_buf,5,4);
				Debug_usart_write(test_buf,10,TEST_DEBUG);
				Debug_usart_write("\t",1,TEST_DEBUG);
			}
		}
	}


	Get_coe_a_b_r(p_7766,pp_6530,16,&coea_pl,&coeb_pl,&coer_pl);
	Get_coe_a_b_r(p_7766,pp_6530,p_cnt,&coea_ph,&coeb_ph,&coer_ph);
	Get_coe_a_b_r(ii_7766,ii_6530,31,&coea_il,&coeb_il,&coer_il);
	Get_coe_a_b_r(&ii_7766[32],&ii_6530[32],4,&coea_i,&coeb_i,&coer_i);

	if((coer_pl > 0.9) && (coer_i > 0.9) && (coer_ph > 0.9) && (coer_il > 0.9) )
	{
		if(ac6530_com_v > 200)
		{
			Debug_usart_write("write220 flash\r\n",16,TEST_DEBUG);
#if 1
			self_adjust_comv = ac6530_com_v - self_com_v;
			self_adjust_coea_pl = coea_pl;
			self_adjust_coeb_pl = coeb_pl;
			self_adjust_coer_pl = coer_pl;

			self_adjust_coea_ph = coea_ph;
			self_adjust_coeb_ph = coeb_ph;
			self_adjust_coer_ph = coer_ph;

			self_adjust_coea_i = coea_i;
			self_adjust_coeb_i = coeb_i;
			self_adjust_coer_i = coer_i;

			self_adjust_coea_il = coea_il;
			self_adjust_coeb_il = coeb_il;
			self_adjust_coer_il = coer_il;
			write_coe_from_flash(V_220);
#endif
		}
		else
		{
			Debug_usart_write("write110 flash\r\n",16,TEST_DEBUG);
#if 1
			self_adjust_comv_110 = ac6530_com_v - self_com_v;
			self_adjust_coea_pl_110 = coea_pl;
			self_adjust_coeb_pl_110 = coeb_pl;
			self_adjust_coer_pl_110 = coer_pl;

			self_adjust_coea_ph_110 = coea_ph;
			self_adjust_coeb_ph_110 = coeb_ph;
			self_adjust_coer_ph_110 = coer_ph;

			self_adjust_coea_i_110 = coea_i;
			self_adjust_coeb_i_110 = coeb_i;
			self_adjust_coer_i_110 = coer_i;

			self_adjust_coea_il_110 = coea_il;
			self_adjust_coeb_il_110 = coeb_il;
			self_adjust_coer_il_110 = coer_il;
			write_coe_from_flash(V_110);
#endif
		}


		ret = 1;
	}

	lcd_show_coe_abr_info(coea_pl,coeb_pl,coer_pl);

	memset(ch_sta,0x00, sizeof(ch_sta));
	operate_ch_relay(ch_sta);
	if(ret)
	{
		return 1;
	}

	return 0;
}

int32_t Go_external_adjust(void)
{
	uint8_t ad_sta = EXTERNAL_START;

	uint8_t ch_sta[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//1 2 3 4 5 6 7 8 9 10 60 60 600 600 600 600 600 600
	//uint32_t load_size[28] = {1,2,3,4,5,6,7,8,9,10,15,60,120,600,660,720,1200,1260,1320,1800,1860,1920,2400,2460,2520,3000,3060,3160};
	//uint32_t load_size[16] = {1,3,6,12,20,50,70,100,170,500,600,1000,1500,2000,2500,3000};
	uint32_t load_size[18] = {1,3,6,10,13,16,20,25,30,36,39,50,70,100,500,1000,1500,2000};
	uint32_t load_cnt = 18;
	uint8_t read_count = 0;
	uint8_t read_c76_count = 0;
	uint8_t err_cnt = 0;
	uint32_t len = 0;

	uint8_t c  = 0;
	uint8_t ret = 0;
	uint8_t i = 0,j = 0,k = 0;
	uint8_t p_cnt = 0,v_cnt = 0,i_cnt = 0;
	uint8_t m = 0;
	uint8_t self_buf[120] = {0};
	uint8_t exter_buf[50] = {0};

	uint8_t test_buf[20] = {0};
	uint8_t test_buf_b[10] = {0};

	double ex_coea_p = 0;
	double ex_coeb_p = 0;
	double ex_coer_p = 0;

	double ex_coea_pl = 0;
	double ex_coeb_pl = 0;
	double ex_coer_pl = 0;

	double ex_coea_i = 0;
	double ex_coeb_i = 0;
	double ex_coer_i = 0;

	double ex_coea_il = 0;
	double ex_coeb_il = 0;
	double ex_coer_il = 0;

	double self_p[36] = {0};
	double self_v[36] = {0};
	double self_i[36] = {0};

	double self_pp[36] = {0};
	//double self_vv[28] = {0};
	double self_ii[36] = {0};

	double exter_p[36] = {0};
	double exter_v[36] = {0};
	double exter_i[36] = {0};

	double exter_pp[36] = {0};
	double exter_vv[36] = {0};
	double exter_ii[36] = {0};

	double self_p_tmp[10] = {0};
	double self_v_tmp[10] = {0};
	double self_i_tmp[10] = {0};

	double exter_com_v = 0;
	double self_com_v = 0;

	count_time_flag = 0;
	if(ad_sta==EXTERNAL_START)
	{
		External_usart_write((uint8_t *)"AT+C76START\r\n",13);
		Debug_usart_write((uint8_t *)"AT+C76START\r\n",13,INFO_DEBUG);
		while(1)
		{

			len = pool_recv_one_command(&externalrx,exter_buf,6,EXTERNAL_POOL);
			if(len > 0)
			{
				Debug_usart_write((uint8_t *)"AT+C76START Recv:",17,INFO_DEBUG);
				Debug_usart_write(exter_buf,len,INFO_DEBUG);
				Debug_usart_write((uint8_t *)"\r\n",2,INFO_DEBUG);
				if(strncmp((const char *)exter_buf,(const char *)"\r\nOK",(size_t)4)==0)
				{
					err_cnt = 0;
					ad_sta++;
					break;
				}
			}
			else
			{
#if 0
				if(err_cnt < 3)
				{
					External_usart_write((uint8_t *)"AT+C76START\r\n",13);
					Debug_usart_write((uint8_t *)"AT+C76START\r\n",13,INFO_DEBUG);
					err_cnt++;
				}
				else
				{
					return -3;
				}
#endif
			}

			if(count_time_flag==1)
			{
				read_count++;
				External_usart_write((uint8_t *)"AT+C76START\r\n",13);
				if(read_count >= 3)
				{
					read_count = 0;
					Debug_usart_write("recv c76 outtime\r\n",18,INFO_DEBUG);
					count_time_flag = 0;
					return -3;
				}
				count_time_flag = 0;
			}
		}
	}

	read_count = 0;

	if(ad_sta==EXTERNAL_READ_NOW)
	{
		for(i=0;i<load_cnt;i++)
		{
			change_load_size(ch_sta,load_size[i]);
			operate_ch_relay(ch_sta);
			memset(ch_sta,0x00, sizeof(ch_sta));
			//memset(self_p,0x00, sizeof(self_p));
			//memset(self_v,0x00, sizeof(self_v));
			//memset(self_i,0x00, sizeof(self_i));
			memset(self_buf,0x00, sizeof(self_buf));
			pool_recv_one_command(&externalrx,exter_buf,50,EXTERNAL_POOL);

			k = 0;
			j = 0;
			read_c76_count = 0;
			external_wait_flag = 0;
			while(1)
			{
				Debug_usart_write("wait chx change\r\n",17,'Y');
				if(external_wait_flag==1)
				{
					read_count++;
					if(read_count >= 7)
					{
						read_count = 0;
						break;
					}
					external_wait_flag = 0;
				}
			}

			initDataPool(&externalrx);
			initDataPool(&cse7766rx);
			External_usart_write((uint8_t *)"AT+C76CAPTURE\r\n",15);
			Debug_usart_write((uint8_t *)"AT+C76CAPTURE\r\n",15,'Y');
			while(1)
			{
				memset(exter_buf,0x00, sizeof(exter_buf));
				len = pool_recv_one_command(&externalrx,exter_buf,6,EXTERNAL_POOL);
				if(len > 0)
				{
					Debug_usart_write((uint8_t *)"AT+C76CAPTURE Recv:",19,INFO_DEBUG);
					Debug_usart_write(exter_buf,len,INFO_DEBUG);
					Debug_usart_write((uint8_t *)"\r\n",2,INFO_DEBUG);
					//if(strncmp((const char *)exter_buf,(const char *)"\r\nOK",(size_t)4)==0)
					if(exter_buf[2]=='O' && exter_buf[3]=='K')
					{
						err_cnt = 0;
						break;
					}
					else
					{
						if(err_cnt < 3)
						{
							initDataPool(&externalrx);
							External_usart_write((uint8_t *)"AT+C76CAPTURE\r\n",13);
							Debug_usart_write((uint8_t *)"AT+C76CAPTURE\r\n",13,INFO_DEBUG);
							err_cnt++;
						}
						else
						{
							memset(ch_sta,0x00, sizeof(ch_sta));
							operate_ch_relay(ch_sta);
							return -3;
						}
					}
				}

				if(count_time_flag==1)
				{
					read_count++;
					if(read_count >= 3)
					{
						read_count = 0;
						Debug_usart_write("recv c76 outtime\r\n",18,INFO_DEBUG);
						count_time_flag = 0;
						memset(ch_sta,0x00, sizeof(ch_sta));
						operate_ch_relay(ch_sta);
						return -3;
					}
					count_time_flag = 0;
				}
			}

			initDataPool(&cse7766rx);
			//initDataPool(&externalrx);
			count_time_flag = 0;

			while(1)
			{
				for(j=0;j<5;j++)
				{
					if(load_size[i]>0)
					{
						count_time_flag = 0;
						while(1)
						{
							if(count_time_flag==1)
							{
								read_count++;
								if(read_count >= 10)
								{
									memset(&self_buf[24*j],0x00, 24);
									memset(ch_sta,0x00, sizeof(ch_sta));
									operate_ch_relay(ch_sta);
									break;
								}
								count_time_flag = 0;
							}
							pool_recv_one_command(&cse7766rx,&self_buf[24*j],24,CSE_7766_POOL);
							Debug_usart_write(&self_buf[24*j],24,CSE7766_DEBUG);
							ret = Check_data_is_error(&self_buf[24*j],24);
							if(ret==1)
							{
								break;
							}
						}
					}
				}
				memset(exter_buf,0x00, sizeof(exter_buf));
				len = pool_recv_one_command(&externalrx,exter_buf,50,EXTERNAL_POOL);
				if(len > 0)
				{
					Debug_usart_write("recv c76 pvi:",13,INFO_DEBUG);
					Debug_usart_write(exter_buf,len,INFO_DEBUG);
					Debug_usart_write("\r\n",2,INFO_DEBUG);
					break;
				}
				if(count_time_flag==1)
				{
					read_c76_count++;
					if(read_c76_count >= 15)
					{
						memset(ch_sta,0x00, sizeof(ch_sta));
						operate_ch_relay(ch_sta);
						count_time_flag = 0;
						return -2;
					}
					count_time_flag = 0;
				}
			}

			for(m=0;m<j;m++)
			{
				if(Get_origin_pvi(&self_buf[24*m],24,&self_p_tmp[m],&self_v_tmp[m],&self_i_tmp[m])==1)
				{
					k++;
				}
				else
				{
					self_p_tmp[m] = 0;
					self_v_tmp[m] = 0;
					self_i_tmp[m] = 0;
				}
			}
			//if(k==0)
			//{
			//	return -1;
			//}
			for(m=0;m<j;m++)
			{
				self_p[i] += self_p_tmp[m];
				self_v[i] += self_v_tmp[m];
				self_i[i] += self_i_tmp[m];

				flodou_to_string(self_p_tmp[m],test_buf,5,4);
				Debug_usart_write(test_buf,10,TEST_DEBUG);
				Debug_usart_write("\r\n",2,TEST_DEBUG);
			}

			c = k + '0';
			Debug_usart_write(&c,1,TEST_DEBUG);
			Debug_usart_write("\r\n",2,TEST_DEBUG);

			//flodou_to_string(self_p[i],test_buf,5,4);
			//Debug_usart_write(test_buf,10,TEST_DEBUG);
			//Debug_usart_write("\r\n",2,TEST_DEBUG);

			self_p[i] /= k;
			self_v[i] /= k;
			self_i[i] /= k;

			flodou_to_string(self_i[i],test_buf,5,4);
			Debug_usart_write(test_buf,10,TEST_DEBUG);
			Debug_usart_write("\r\n",2,TEST_DEBUG);

			flodou_to_string(self_p[i],test_buf,5,4);
			Debug_usart_write(test_buf,10,TEST_DEBUG);
			Debug_usart_write("\r\n",2,TEST_DEBUG);

			if(self_v[i] < 200)
			{
				self_v[i] += self_adjust_comv_110;
				if(self_p[i] > 6)
				{
					self_p[i] *= 10;
					self_p[i] = self_p[i]*self_adjust_coea_ph_110+self_adjust_coeb_ph_110;
					self_p[i] /= 10;
				}
				else
				{
					self_p[i] = self_p[i]*self_adjust_coea_pl_110+self_adjust_coeb_pl_110;
				}
				if(self_i[i] > 1)
				{
					self_i[i] = self_i[i]*self_adjust_coea_i_110+self_adjust_coeb_i_110;
				}
				else
				{
					//self_i[i] = self_i[i]*self_adjust_coea_il_110+self_adjust_coeb_il_110;
					self_i[i] = self_i[i]*self_adjust_coea_i;
				}
			}
			else
			{
				self_v[i] += self_adjust_comv;
				if(self_p[i] > 25)
				{
					self_p[i] = self_p[i]*self_adjust_coea_ph+self_adjust_coeb_ph;
				}
				else
				{
					self_p[i] = self_p[i]*self_adjust_coea_pl+self_adjust_coeb_pl;
				}
				if(self_i[i] > 2)
				{
					self_i[i] = self_i[i]*self_adjust_coea_i+self_adjust_coeb_i;
				}
				else
				{
					//self_i[i] = self_i[i]*self_adjust_coea_il+self_adjust_coeb_il;
					self_i[i] = self_i[i]*self_adjust_coea_il;
				}
			}



			flodou_to_string(self_p[i],test_buf,4,4);
			Debug_usart_write(test_buf,9,TEST_DEBUG);
			Debug_usart_write("\r\n",2,TEST_DEBUG);

			if(Get_computer_external_pvi(exter_buf,len,&exter_p[i],&exter_v[i],&exter_i[i])==0)
			{
				return -4;
			}

			flodou_to_string(exter_p[i],test_buf,4,4);
			Debug_usart_write(test_buf,9,TEST_DEBUG);
			Debug_usart_write("\r\n\r\n",4,TEST_DEBUG);


			lcd_change_percent_info(6*i);
		}

		lcd_change_percent_info(100);
#if 1
		Debug_usart_write("\r\n",2,TEST_DEBUG);
#endif
		for(m=0;m<load_cnt;m++)
		{
			if(exter_p[m] > 0.0001 && self_p[m] > 0.0001)
			{
				if((self_p[m] > exter_p[m]*0.7) && (self_p[m] < exter_p[m]*1.3))
				{
					exter_pp[p_cnt] = exter_p[m];
					self_pp[p_cnt] = self_p[m];
					p_cnt++;
				}
			}
			if(exter_i[m] > 0.0001 && self_i[m] > 0.0001)
			{
				if((self_i[m] > exter_i[m]*0.5) && (self_i[m] < exter_i[m]*1.5))
				{
					exter_ii[i_cnt] = exter_i[m];
					self_ii[i_cnt] = self_i[m];
					i_cnt++;
				}
			}
			if(self_v[m] > 0.0001 && exter_v[m] > 0.0001)
			{
				if((self_v[m] > exter_v[m]*0.7) && (self_v[m] < exter_v[m]*1.3))
				{
					k++;
					exter_com_v += exter_v[m];
					self_com_v += self_v[m];
				}
			}
		}

		for(m=0;m<p_cnt;m++)
		{
			flodou_to_string(exter_pp[m],test_buf_b,Get_double_mantissa_len(&exter_pp[m]),3);
			Debug_usart_write(test_buf_b,9,TEST_DEBUG);
			Debug_usart_write("\r\n",2,TEST_DEBUG);
		}
		Debug_usart_write("\r\n",2,TEST_DEBUG);
		for(m=0;m<p_cnt;m++)
		{
			flodou_to_string(self_pp[m],test_buf_b,Get_double_mantissa_len(&self_pp[m]),3);
			Debug_usart_write(test_buf_b,9,TEST_DEBUG);
			Debug_usart_write("\r\n",2,TEST_DEBUG);
		}
		Debug_usart_write("\r\n",2,TEST_DEBUG);
		for(m=0;m<i_cnt;m++)
		{
			flodou_to_string(exter_ii[m],test_buf,Get_double_mantissa_len(&exter_ii[m]),3);
			Debug_usart_write(test_buf,9,TEST_DEBUG);
			Debug_usart_write("\r\n",2,TEST_DEBUG);
		}

		Debug_usart_write("\r\n",2,TEST_DEBUG);

		for(m=0;m<i_cnt;m++)
		{
			flodou_to_string(self_ii[m],test_buf_b,Get_double_mantissa_len(&self_ii[m]),3);
			Debug_usart_write(test_buf_b,9,TEST_DEBUG);
			Debug_usart_write("\r\n",2,TEST_DEBUG);
		}

		Debug_usart_write("\r\n",2,TEST_DEBUG);

		exter_com_v /= k;
		self_com_v /= k;
		external_adjust_comv = self_com_v - exter_com_v;

		Get_coe_a_b_r(exter_pp,self_pp,8,&ex_coea_pl,&ex_coeb_pl,&ex_coer_pl);
		Get_coe_a_b_r(exter_pp,self_pp,p_cnt,&ex_coea_p,&ex_coeb_p,&ex_coer_p);
		Get_coe_a_b_r(exter_ii,self_ii,i_cnt-4,&ex_coea_il,&ex_coeb_il,&ex_coer_il);
		Get_coe_a_b_r(&exter_ii[i_cnt-3],&self_ii[i_cnt-3],3,&ex_coea_i,&ex_coeb_i,&ex_coer_i);

		//ex_coeb_i = 0;

		if((ex_coer_p > 0.9) && (ex_coer_i > 0.9) && (ex_coer_pl > 0.9) && (ex_coer_il > 0.9))
		{
			if((ex_coea_p > 0.7 && ex_coea_p < 1.5) && (ex_coea_i > 0.7 && ex_coea_i < 1.5))
			{
				if((ex_coeb_p > -2 && ex_coeb_p < 2) && (ex_coeb_i > -0.05 && ex_coeb_i < 0.05))
				{
					Send_result_to_c76(1,ex_coea_pl,ex_coeb_pl,ex_coea_il,ex_coeb_il,external_adjust_comv,
										ex_coea_p,ex_coeb_p,ex_coea_i,ex_coeb_i,external_adjust_comv);
					ret = 1;
				}
				else
				{
					Send_result_to_c76(0,ex_coea_pl,ex_coeb_pl,ex_coea_il,ex_coeb_il,external_adjust_comv,
										ex_coea_p,ex_coeb_p,ex_coea_i,ex_coeb_i,external_adjust_comv);
					ret = 0;
				}
			}
			else
			{
				Send_result_to_c76(0,ex_coea_pl,ex_coeb_pl,ex_coea_il,ex_coeb_il,external_adjust_comv,
									ex_coea_p,ex_coeb_p,ex_coea_i,ex_coeb_i,external_adjust_comv);
				ret = 0;
			}
		}
		else
		{
			Send_result_to_c76(0,ex_coea_pl,ex_coeb_pl,ex_coea_il,ex_coeb_il,external_adjust_comv,
								ex_coea_p,ex_coeb_p,ex_coea_i,ex_coeb_i,external_adjust_comv);
			ret = 0;
		}

	}
	else
	{
		return -3;
	}


	lcd_show_coe_allab_info(ex_coea_p,ex_coeb_p,ex_coea_i,ex_coeb_i);

	if(ret==1)
	{
			count_time_flag = 0;
			while(1)
			{
				len = pool_recv_one_command(&externalrx,test_buf,50,EXTERNAL_POOL);
				if(len>0)
				{
					Debug_usart_write("c76 adj ret:",12,TEST_DEBUG);
					Debug_usart_write(test_buf,len,TEST_DEBUG);
					Debug_usart_write("\r\n",2,TEST_DEBUG);
					break;
				}
				if(count_time_flag==1)
				{
					read_count++;
					if(read_count >= 5)
					{
						memset(ch_sta,0x00, sizeof(ch_sta));
						operate_ch_relay(ch_sta);
						count_time_flag = 0;
						return 1;
					}
					count_time_flag = 0;
				}
			}
	}

	memset(ch_sta,0x00, sizeof(ch_sta));
	operate_ch_relay(ch_sta);

	count_time_flag = 0;
	read_count = 0;
	Debug_usart_write("come\r\n",6,TEST_DEBUG);
	while(1)
	{
		if(count_time_flag==1)
		{
			read_count++;
			if(read_count >= 2)
			{
				count_time_flag = 0;
				break;
			}
			count_time_flag = 0;
		}
	}
	Debug_usart_write("out\r\n",5,TEST_DEBUG);


	if(ret==1)
	{
		return 1;
	}

	return 0;
}

void Check_c76_fac(void)
{
	uint8_t i = 0;
	uint32_t len = 0;
	uint8_t exter_buf[10] = {0};

	len = pool_recv_one_command(&externalrx,exter_buf,50,C76_10MS_TIME);
	if(len > 0)
	{
		if(exter_buf[0]=='f')
		{
			External_usart_write((uint8_t *)"factory!",8);
			Debug_usart_write((uint8_t *)"send fac over\r\n",15,INFO_DEBUG);
		}
		Debug_usart_write("c76 start recv:",15,INFO_DEBUG);
		Debug_usart_write(exter_buf,len,INFO_DEBUG);
		Debug_usart_write("\r\n",2,INFO_DEBUG);
	}

}

void relay_test(void)
{
	static uint8_t i = 0;
	uint8_t buf[50] = {0};
	uint32_t len = 0;
	uint8_t ch_sta[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint32_t load_size[36] = {1,3,4,6,8,10,11,13,14,15,16,18,20,21,23,25,26,28,30,33,34,36,38,
								  39,50,60,70,80,100,130,139,500,1000,1500,2000};
#if 1
	change_load_size(ch_sta,load_size[i]);
	operate_ch_relay(ch_sta);
	i++;
	if(i==36)
	{
		i = 0;
	}
#endif
	//AC6530_usart_write((uint8_t *)"MEASure:CURRent:AC?\r\n",21);
	//len = pool_recv_one_command(&externalrx,buf,200,EXTERNAL_WAIT_TIME);
	//Debug_usart_write(buf,len,INFO_DEBUG);

	//AC6530_usart_write((uint8_t *)"factory?",8);
	//External_usart_write((uint8_t *)"factory?",8);
}

int main()
{
	//double x[11] = {0.226856,0.444463,2.163232,2.377381,2.589446,4.273389,4.493904,
	//				6.365462,6.563147,8.465073,10.510610};
	//double y[11] = {0.26,0.47,2.24, cdww2.46,2.68,4.4,4.64,6.58,6.67,8.71,10.84};

	double p=0,v=0,i=0;
	uint32_t len = 0;
	uint8_t ch_sta[16] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t data[500] = {0};
	uint8_t err_code[2] = {0};
	uint8_t ret = 0;
	int32_t ad_ret = 0;
	uint8_t exce_cnt = 0;
	uint8_t noadj_cnt = 0;
	uint8_t cycle_cnt = 0;
	uint8_t exter_err_cnt = 0;

	uint32_t time_cnt = 0;

	system_clk_init();

	CSE7766_usart_init();
	AC6530_usart_init();
	External_usart_init();
	Debug_usart_init();

	source_relay_pin_init();
	load_relay_pin_init();
	tim_init();
	initDataPool(&cse7766rx);
	initDataPool(&ac6530rx);
	initDataPool(&externalrx);

	iic_2864_ssd1306_init();

	lcd_show_pvi_info(0,0,0);

	//write_coe_from_flash();

	//operate_ch_relay(ch_sta);
	//relay_test();
	while (1)
    {
#if 0
		if(count_time_flag==1)
		{
			len++;
			if(len == 2)
			{
				len = 0;
				relay_test();
			}
			count_time_flag = 0;
		}
#else
		Check_c76_fac();
		key_operate();
		if(operate_mode==SELF_ADJUST_MODE)
		{
			Debug_usart_write("Come to self_adjust mode\r\n",27,INFO_DEBUG);
			lcd_show_local_now_adjust_info();
			ad_ret = Go_self_adjust();
			if(ad_ret == 1)
			{
				Debug_usart_write("self_adjust OK\r\n",16,INFO_DEBUG);
				self_ad_ok_flag = 1;
			}
			else
			{
				if(ad_ret == -3)
				{
					Debug_usart_write("self_adjust read 7766 failed\r\n",30,INFO_DEBUG);
					lcd_show_self_read7766_error_info();
				}
				else if(ad_ret == -1)
				{
					Debug_usart_write("self_adjust read 6530 failed\r\n",30,INFO_DEBUG);
					lcd_show_self_read6530_error_info();
				}
				else if(ad_ret == -2)
				{
					Debug_usart_write("self_adjust 7766 data nok\r\n",27,INFO_DEBUG);
				}
				else if(ad_ret == 0)
				{
					Debug_usart_write("self_adjust R nok\r\n",19,INFO_DEBUG);
				}
			}
			operate_mode = 4;
		}
		else if(operate_mode==EXTERNAL_ADJUST_MODE)
		{
			initDataPool(&cse7766rx);
			if(self_ad_ok_flag==1)
			{
				Debug_usart_write("Come to external_adjust mode\r\n",30,INFO_DEBUG);
				lcd_show_exter_now_adjust_info();
				ad_ret = Go_external_adjust();
				Debug_usart_write("external_adjust over\r\n",22,INFO_DEBUG);
				if(ad_ret == -3)
				{
					exter_err_cnt = 0;
					operate_mode = 5;
					lcd_show_recvc76_err_info();
				}
				if(ad_ret == -1)
				{
					exter_err_cnt = 0;
					operate_mode = 5;
					Debug_usart_write("read 7766 err.....\r\n",20,INFO_DEBUG);
					lcd_show_exter_read7766_error_info();
				}
				if(ad_ret == -2)
				{
					exter_err_cnt = 0;
					operate_mode = 5;
					lcd_show_exter_readc76pvi_error_info();
				}
				if(ad_ret == -4)
				{
					exter_err_cnt = 0;
					operate_mode = 5;
					lcd_show_exter_c76pvi_data_error_info();
				}
				if(ad_ret == 0)
				{
					exter_err_cnt++;
					if(exter_err_cnt == 3)
					{
						exter_err_cnt = 0;
						operate_mode = 5;
						lcd_show_exter_stop_info();
					}
				}
				if(ad_ret == 1)
				{
					exter_err_cnt = 0;
					operate_mode = 5;
				}
			}
			else
			{
				lcd_show_local_noadjust_info();
			}
			//Debug_usart_write("set mode 5\r\n",12,INFO_DEBUG);
		}
		else if(operate_mode==0)
		{
			if(self_ad_ok_flag)
			{
				//Debug_usart_write("come\r\n",6,'Y');
				len = pool_recv_one_command(&cse7766rx,data,500,CSE_7766_POOL);
				//Debug_usart_write("out\r\n",5,'Y');
				//Debug_usart_write(data,len,DATA_SEND_RECV_DABUG);
				if(count_time_flag==1)
				{
#if 0
					time_cnt++;
					if(time_cnt >= 150)
					{
						relay_test();
						time_cnt = 0;
					}
#endif
					//len = pool_recv_one_command(&cse7766rx,data,500,CSE_7766_POOL);
					Debug_usart_write(data,len,DATA_SEND_RECV_DABUG);
					if(len > 0)
					{
						exce_cnt = 0;
						ret = Get_compute_pvi(data,len,&p,&v,&i);
						if(ret==1)
						{
							Debug_usart_write("recv pvi ok...\r\n",16,INFO_DEBUG);
							lcd_show_pvi_info(p,v,i);
						}
						else
						{
							if(ret == 0xAA)
							{
								noadj_cnt++;
								if(noadj_cnt >= 10)
								{
									noadj_cnt = 0;
									Debug_usart_write("7766 data no adjust...\r\n",24,INFO_DEBUG);
									lcd_show_noad_info();
								}
							}
							else if(ret >= 0xF0 && ret < 0xFF)
							{
								exce_cnt++;
								if(exce_cnt >= 10)
								{
									exce_cnt = 0;
									Debug_usart_write("7766 data error code:",21,INFO_DEBUG);
									hex_to_str(err_code,ret);
									Debug_usart_write(err_code,2,INFO_DEBUG);
									Debug_usart_write("\r\n",2,INFO_DEBUG);
									lcd_show_err_code_info(ret);
								}
							}
							else
							{
								cycle_cnt++;
								if(cycle_cnt>=10)
								{
									cycle_cnt = 0;
									Debug_usart_write("adj cycle incomplete:",21,INFO_DEBUG);
									hex_to_str(err_code,ret);
									Debug_usart_write(err_code,2,INFO_DEBUG);
									Debug_usart_write("\r\n",2,INFO_DEBUG);
								}
							}
						}
					}
					else
					{
						exce_cnt++;
						if(exce_cnt >= 5)
						{
							exce_cnt = 0;
							lcd_show_no_return_info();
						}
					}
					count_time_flag = 0;
				}
				memset(data,0x00, sizeof(data));
			}
			else
			{
				ret = read_coe_from_flash();
				if(ret)
				{
					self_ad_ok_flag = 1;
					Debug_usart_write("Get coe ok\r\n",12,INFO_DEBUG);
					Debug_write_coe_info();
					Debug_write_coe110_info();
				}
				else
				{
					Debug_usart_write("Get coe nok\r\n",13,INFO_DEBUG);
					lcd_show_local_noadjust_info();
				}
			}
		}
#endif
    }


}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
