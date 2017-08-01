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

uint8_t operate_mode = 0;
uint8_t self_ad_ok_flag = 0;
uint8_t external_ad_ok_flag = 0;

GPIO_TypeDef* ch_io[16] = {GPIOA,GPIOC,GPIOC,GPIOB,GPIOB,GPIOB,GPIOB,GPIOB,GPIOB,GPIOC,GPIOC,GPIOA,GPIOA,GPIOA,GPIOA,GPIOA};
uint16_t	ch_pin[16] = {GPIO_Pin_8,GPIO_Pin_7,GPIO_Pin_6,GPIO_Pin_15,GPIO_Pin_14,GPIO_Pin_13,GPIO_Pin_12,GPIO_Pin_1,
						  GPIO_Pin_0,GPIO_Pin_5,GPIO_Pin_4,GPIO_Pin_7,GPIO_Pin_6,GPIO_Pin_5,GPIO_Pin_4,GPIO_Pin_1};

double self_adjust_coea_p = 0;
double self_adjust_coeb_p = 0;
double self_adjust_coer_p = 0;

double self_adjust_coea_i = 0;
double self_adjust_coeb_i = 0;
double self_adjust_coer_i = 0;

double external_adjust_coea_p = 0;
double external_adjust_coeb_p = 0;
double external_adjust_coer_p = 0;

double external_adjust_coea_i = 0;
double external_adjust_coeb_i = 0;
double external_adjust_coer_i = 0;

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
	uint8_t p_6530[10] = {"69.05"};
	uint8_t v_6530[10] = {"220.16"};
	uint8_t i_6530[10] = {"2.30"};

	uint32_t p_len = 0;
	uint32_t v_len = 0;
	uint32_t i_len = 0;

	uint8_t cnt = 0;

	for(cnt=0;cnt<3;cnt++)
	{
		External_usart_write((uint8_t *)"MEASure:CURRent:AC?",19);
		p_len = pool_recv_one_command(&externalrx,i_6530,10,EXTERNAL_POOL);
		if(p_len > 0)
		{
			break;
		}
		//if(cnt == 2)
		//{
		//	return 0;
		//}
	}

	for(cnt=0;cnt<3;cnt++)
	{
		External_usart_write((uint8_t *)"MEASure:VOLTage:AC?",19);
		v_len = pool_recv_one_command(&externalrx,v_6530,10,EXTERNAL_POOL);
		if(v_len > 0)
		{
			break;
		}
		//if(cnt == 2)
		//{
		//	return 0;
		//}
	}

	for(cnt=0;cnt<3;cnt++)
	{
		External_usart_write((uint8_t *)"MEASure:POWer:AC?",17);
		i_len = pool_recv_one_command(&externalrx,p_6530,10,EXTERNAL_POOL);
		if(i_len > 0)
		{
			break;
		}
		//if(cnt == 2)
		//{
		//	return 0;
		//}
	}

	//string_to_flodou(p,p_6530,p_len);
	//string_to_flodou(v,v_6530,v_len);
	//string_to_flodou(i,i_6530,i_len);

	string_to_flodou(p,p_6530,5);
	string_to_flodou(v,v_6530,6);
	string_to_flodou(i,i_6530,3);

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

	return 0x01;
}
#endif


uint8_t Get_compute_pvi(uint8_t *data,uint32_t len,double *ret_p,double *ret_v,double *ret_i)
{
	double p=0,v=0,i=0;
	uint8_t ret = 0;

	ret = Get_origin_pvi(data,len,&p,&v,&i);
	if(ret==1)
	{
		*ret_p = p*self_adjust_coea_p+self_adjust_coeb_p;
		*ret_i = i*self_adjust_coea_i+self_adjust_coeb_i;
		*ret_v = v;
		return 1;
	}

	return ret;
}


void Debug_write_coe_info(void)
{
	uint8_t buf[10] = {0};
	double p_a = self_adjust_coea_p;
	double p_b = self_adjust_coeb_p;
	double p_r = self_adjust_coer_p;
	double i_a = self_adjust_coea_i;
	double i_b = self_adjust_coeb_i;
	double i_r = self_adjust_coer_i;

	Debug_usart_write("P_A:",4,INFO_DEBUG);
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

	Debug_usart_write("P_B:",4,INFO_DEBUG);
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

	Debug_usart_write("P_R:",4,INFO_DEBUG);
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
	Debug_usart_write("\r\n",2,INFO_DEBUG);

	Debug_usart_write("I_A:",4,INFO_DEBUG);
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
}

void key_operate(void)
{
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
		    	relay_sta = ~relay_sta;
	    	}
	    	if(SELF_ADJUST_KEY_READ==0)
	    	{
	    		if(operate_mode == SELF_ADJUST_MODE)
	    			operate_mode = 0;
	    		else
	    			operate_mode = SELF_ADJUST_MODE;
	    	}
	    	if(EXTERNAL_KEY_READ==0)
	    	{
	    		if(operate_mode == EXTERNAL_ADJUST_MODE)
	    			operate_mode = 0;
	    		else
	    			operate_mode = EXTERNAL_ADJUST_MODE;
	    	}
	    }
	    key_backup = key_status;
	}
}

void change_load_size(uint8_t *ch_sta,uint32_t load_size)
{
	if(load_size==1)
					ch_sta[0] = 1;
	else if(load_size==2)
					ch_sta[1] = 1;
	else if(load_size==3)
					ch_sta[2] = 1;
	else if(load_size==4)
					ch_sta[3] = 1;
	else if(load_size==5)
					ch_sta[4] = 1;
	else if(load_size==6)
					ch_sta[5] = 1;
	else if(load_size==7)
					ch_sta[6] = 1;
	else if(load_size==8)
					ch_sta[7] = 1;
	else if(load_size==9)
					ch_sta[8] = 1;
	else if(load_size==10)
					ch_sta[9] = 1;
	else if(load_size==15)
	{
					ch_sta[4] = 1;ch_sta[9] = 1;
	}
	else if(load_size==60)
					ch_sta[10] = 1;
	else if(load_size==120)
	{
					ch_sta[10] = 1;ch_sta[11] = 1;
	}
	else if(load_size==600)
	{
					ch_sta[12] = 1;
	}
	else if(load_size==660)
	{
					ch_sta[10] = 1;ch_sta[12] = 1;
	}
	else if(load_size==720)
	{
					ch_sta[10] = 1;ch_sta[11] = 1;ch_sta[12] = 1;
	}
	else if(load_size==1200)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;
	}
	else if(load_size==1260)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[10] = 1;
	}
	else if(load_size==1320)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[10] = 1;ch_sta[11] = 1;
	}
	else if(load_size==1800)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;
	}
	else if(load_size==1860)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;ch_sta[10] = 1;
	}
	else if(load_size==1920)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;ch_sta[10] = 1;ch_sta[11] = 1;
	}
	else if(load_size==2400)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;ch_sta[15] = 1;
	}
	else if(load_size==2460)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;ch_sta[15] = 1;ch_sta[10] = 1;
	}
	else if(load_size==2520)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;ch_sta[15] = 1;ch_sta[10] = 1;ch_sta[11] = 1;
	}
	else if(load_size==3000)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;ch_sta[15] = 1;ch_sta[16] = 1;
	}
	else if(load_size==3160)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;ch_sta[15] = 1;ch_sta[16] = 1;ch_sta[10] = 1;
	}
	else if(load_size==3220)
	{
					ch_sta[12] = 1;ch_sta[13] = 1;ch_sta[14] = 1;ch_sta[15] = 1;ch_sta[16] = 1;ch_sta[10] = 1;ch_sta[11] = 1;
	}
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

uint32_t Get_computer_external_pvi(uint8_t *data,uint32_t len,double *p,double *v,double *i)
{
	uint8_t tmp = 0;
	uint8_t m = 0,n = 0;;
	uint8_t p_buf[10] = {0};
	uint8_t v_buf[10] = {0};
	uint8_t i_buf[10] = {0};
	double p_ret = 0,v_ret = 0,i_ret = 0;


	for(tmp=0;tmp<len;tmp++)
	{
		if(data[tmp] == '\r')
		{
			break;
		}
		if(data[tmp] == ',')
		{
			m++;
			n = 0;
			break;
		}
		if(m==0)
		{
			p_buf[n] = data[tmp];
			n++;
		}
		if(m==1)
		{
			v_buf[n] = data[tmp];
			n++;
		}
		if(m==2)
		{
			i_buf[n] = data[tmp];
			n++;
		}
	}

	string_to_flodou(&p_ret,p_buf,str_len(p_buf));
	string_to_flodou(&v_ret,v_buf,str_len(p_buf));
	string_to_flodou(&i_ret,i_buf,str_len(p_buf));

	*p = p_ret/1000;
	*v = v_ret/1000;
	*i = i_ret/1000;

	return 1;
}

void Send_result_to_c76(uint8_t result,double p_a,double p_b,double i_a,double i_b,double v_coe)
{
	uint8_t send_data[50] = {"AT+C76END="};
	uint8_t p_a_buf[10] = {0};
	uint8_t p_b_buf[10] = {0};
	uint8_t i_a_buf[10] = {0};
	uint8_t i_b_buf[10] = {0};
	uint8_t v_coe_buf[10] = {'0'};

	double p_a_tmp = p_a;
	double p_b_tmp = p_b;
	double i_a_tmp = i_a;
	double i_b_tmp = i_b;
	double v_coe_tmp = v_coe;

	if(result==1)
	{
		send_data[10] = '1';
	}
	else
	{
		send_data[10] = '0';
	}

	if(p_a_tmp<0)
			p_a_tmp = -p_a_tmp;
	if(p_b_tmp<0)
		p_b_tmp = -p_b_tmp;
	if(i_a_tmp<0)
			i_a_tmp = -i_a_tmp;
	if(i_b_tmp<0)
			i_b_tmp = -i_b_tmp;

	p_a_tmp *= 1000;
	p_b_tmp *= 1000;
	i_a_tmp *= 1000;
	i_b_tmp *= 1000;

	if(((uint32_t)p_a_tmp)%10 > 5)
		p_a_tmp += 10;
	if(((uint32_t)p_b_tmp)%10 > 5)
		p_b_tmp += 10;
	if(((uint32_t)i_a_tmp)%10 > 5)
		i_a_tmp += 10;
	if(((uint32_t)i_b_tmp)%10 > 5)
		i_b_tmp += 10;

	p_a_tmp /= 10;
	p_b_tmp /= 10;
	i_a_tmp /= 10;
	i_b_tmp /= 10;

	Itoa((int32_t)p_a_tmp,p_a_buf);
	Itoa((int32_t)p_b_tmp,p_b_buf);
	Itoa((int32_t)i_a_tmp,i_a_buf);
	Itoa((int32_t)i_b_tmp,i_b_buf);

	str_cat(send_data,str_len(send_data),p_a_buf,str_len(p_a_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);
	str_cat(send_data,str_len(send_data),p_b_buf,str_len(p_b_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);
	str_cat(send_data,str_len(send_data),i_a_buf,str_len(i_a_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);
	str_cat(send_data,str_len(send_data),i_b_buf,str_len(i_b_buf));
	str_cat(send_data,str_len(send_data),(uint8_t *)",",(uint32_t)1);
	str_cat(send_data,str_len(send_data),v_coe_buf,str_len(v_coe_buf));

	External_usart_write(send_data,str_len(send_data));
	External_usart_write((uint8_t *)"\r\n",2);
}

int32_t Go_self_adjust(void)
{
	uint8_t ch_sta[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//1 2 3 4 5 6 7 8 9 10 60 60 600 600 600 600 600 600
	//uint32_t load_size[28] = {1,2,3,4,5,6,7,8,9,10,15,60,120,600,660,720,1200,1260,1320,1800,1860,1920,2400,2460,2520,3000,3060,3160};
	uint32_t load_size[17] = {1,2,3,4,5,6,7,8,9,10,15,120,600,1200,1800,2400,3000};
	uint32_t load_cnt = 17;
	uint8_t i = 0,j = 0,z = 0,k = 0;
	uint8_t m = 0;
	uint8_t ret = 0;
	uint8_t data[240] = {0};
	uint8_t cnt_buf[2] = {0};

	uint8_t test_buf[20] = {0};
	uint32_t y = 0;
	uint8_t c = 0;

	double coea_p = 0;
	double coeb_p = 0;
	double coer_p = 0;

	double coea_i = 0;
	double coeb_i = 0;
	double coer_i = 0;

	double p_7766[28] = {0};
	double v_7766[28] = {0};
	double i_7766[28] = {0};

	double p_6530[28] = {0};
	double v_6530[28] = {0};
	double i_6530[28] = {0};

	double p_tmp_6530[3] = {0};
	double v_tmp_6530[3] = {0};
	double i_tmp_6530[3] = {0};

	double p_tmp_7766[10] = {0};
	double v_tmp_7766[10] = {0};
	double i_tmp_7766[10] = {0};

	for(i=0;i<load_cnt;i++)
	{
		change_load_size(ch_sta,load_size[i]);
		operate_ch_relay(ch_sta);
		initDataPool(&cse7766rx);
		z = 0;
		k = 0;
		for(j=0;j<10;j++)
		{
			if((j%4)==0)
			{
				if(Get_6530_pvi(&p_tmp_6530[j/4],&v_tmp_6530[j/4],&i_tmp_6530[j/4])==1)
				{
					k++;
				}
			}
			if(load_size[i]<=10)
			{
				for(m=0;m<50;m++)
				{
					pool_recv_one_command(&cse7766rx,&data[24*j],24,CSE_7766_POOL);
					ret = Check_data_is_error(&data[24*j],24);
					if(ret==1)
					{
						break;
					}
					if(m==49)
					{
						return -3;
					}
				}
			}
			else
			{
				pool_recv_one_command(&cse7766rx,&data[24*j],24,CSE_7766_POOL);
			}

		}
		if(i==0)
		{
			;//Debug_usart_write(data,240,'Y');
		}
		if(k==0)
		{
			//Debug_usart_write(data,24,'Y');
			return -1;
		}
		Debug_usart_write("load_cnt:",9,INFO_DEBUG);
		cnt_buf[0] = (i/10+'0');
		cnt_buf[1] = (i%10+'0');
		Debug_usart_write(cnt_buf,2,INFO_DEBUG);
		Debug_usart_write("\r\n",2,INFO_DEBUG);

		p_6530[i] = (p_tmp_6530[0]+p_tmp_6530[1]+p_tmp_6530[2]);
		v_6530[i] = (v_tmp_6530[0]+v_tmp_6530[1]+v_tmp_6530[2]);
		i_6530[i] = (i_tmp_6530[0]+i_tmp_6530[1]+i_tmp_6530[2]);

		flodou_to_string(p_6530[i],test_buf,4,4);
		Debug_usart_write(test_buf,9,'Y');
		Debug_usart_write("\r\n",2,'Y');

		p_6530[i] /= k;
		v_6530[i] /= k;
		i_6530[i] /= k;

		flodou_to_string(p_6530[i],test_buf,4,4);
		Debug_usart_write(test_buf,9,'Y');
		Debug_usart_write("\r\n",2,'Y');

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
			//flodou_to_string(p_tmp_7766[j],test_buf,2,4);
			//Debug_usart_write(test_buf,7,'Y');
			//Debug_usart_write("\r\n",2,'Y');
		}
		if(z==0)
		{
			return -2;
		}
		for(j=0;j<10;j++)
		{
			p_7766[i] += p_tmp_7766[j];
			v_7766[i] += v_tmp_7766[j];
			i_7766[i] += i_tmp_7766[j];
		}

		flodou_to_string(p_7766[i],test_buf,4,4);
		Debug_usart_write(test_buf,9,'Y');
		Debug_usart_write("\r\n",2,'Y');

		p_7766[i] /= z;
		v_7766[i] /= z;
		i_7766[i] /= z;

		c = z+'0';
		Debug_usart_write(&c,1,'Y');
		Debug_usart_write("  ",2,'Y');

		flodou_to_string(p_7766[i],test_buf,4,4);
		Debug_usart_write(test_buf,9,'Y');
		Debug_usart_write("\r\n",2,'Y');

		if(6*i < 100)
		{
			lcd_change_percent_info(6*(i));
		}
#if 0
		for(y=0;y<10;y++)
		{
			flodou_to_string(p_tmp_7766[y],test_buf,2,4);
			Debug_usart_write(test_buf,7,'Y');
			Debug_usart_write("\r\n",2,'Y');
		}
		Debug_usart_write("\r\n",2,'Y');
		for(y=0;y<10;y++)
		{
			flodou_to_string(p_tmp_6530[y],test_buf,2,4);
			Debug_usart_write(test_buf,7,'Y');
			Debug_usart_write("\r\n",2,'Y');
		}
#endif

		Debug_usart_write("\r\n",2,INFO_DEBUG);
	}
#if 0
	for(i=0;i<17;i++)
	{
		flodou_to_string(p_7766[i],data,2,4);
		Debug_usart_write(data,7,'Y');
		Debug_usart_write("\r\n",2,'Y');
	}
	Debug_usart_write("\r\n",2,'Y');
	for(i=0;i<17;i++)
	{
		flodou_to_string(p_6530[i],data,2,4);
		Debug_usart_write(data,7,'Y');
		Debug_usart_write("\r\n",2,'Y');
	}
#endif

	lcd_change_percent_info(100);
	Get_coe_a_b_r(p_7766,p_6530,28,&coea_p,&coeb_p,&coer_p);
	Get_coe_a_b_r(i_7766,i_6530,28,&coea_i,&coeb_i,&coer_i);

	if((coer_p > 0.9) && (coer_i > 0.9))
	{
		self_adjust_coea_p = coea_p;
		self_adjust_coeb_p = coeb_p;
		self_adjust_coer_p = coer_p;
		self_adjust_coea_i = coea_i;
		self_adjust_coeb_i = coeb_i;
		self_adjust_coer_i = coer_i;
		write_coe_from_flash();
		ret = 1;
	}

	lcd_show_coe_abr_info(coea_p,coeb_p,coer_p);

	if(ret)
	{
		return 1;
	}
	return 0;
}

int32_t Go_external_adjust(void)
{
	static uint8_t ad_sta = 0;

	uint8_t ch_sta[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//1 2 3 4 5 6 7 8 9 10 60 60 600 600 600 600 600 600
	uint32_t load_size[28] = {1,2,3,4,5,6,7,8,9,10,15,60,120,600,660,720,1200,1260,1320,1800,1860,1920,2400,2460,2520,3000,3060,3160};
	uint32_t load_cnt = 28;

	uint32_t len = 0;

	uint8_t ret = 0;
	uint8_t i = 0,j = 0,k = 0;
	uint8_t m = 0;
	uint8_t self_buf[240] = {0};
	uint8_t exter_buf[50] = {0};

	double ex_coea_p = 0;
	double ex_coeb_p = 0;
	double ex_coer_p = 0;

	double ex_coea_i = 0;
	double ex_coeb_i = 0;
	double ex_coer_i = 0;

	double self_p[28] = {0};
	double self_v[28] = {0};
	double self_i[28] = {0};

	double exter_p[28] = {0};
	double exter_v[28] = {0};
	double exter_i[28] = {0};

	double self_p_tmp[10] = {0};
	double self_v_tmp[10] = {0};
	double self_i_tmp[10] = {0};

	if(ad_sta==EXTERNAL_START)
	{
		External_usart_write((uint8_t *)"AT+C76START\r\n",13);
		len = pool_recv_one_command(&externalrx,exter_buf,50,EXTERNAL_POOL);
		if(len > 0)
		{
			if(strncmp((const char *)exter_buf,(const char *)"OK",(size_t)2)==0)
			{
				ad_sta++;
			}
		}
		else
		{
			return 0;
		}
	}

	if(ad_sta==EXTERNAL_READ_NOW)
	{
		for(i=0;i<load_cnt;i++)
		{
			change_load_size(ch_sta,load_size[i]);
			operate_ch_relay(ch_sta);
			pool_recv_one_command(&externalrx,exter_buf,50,EXTERNAL_POOL);
			External_usart_write((uint8_t *)"AT+C76CAPTURE\r\n",13);
			while(1)
			{
				if(load_size[i] <= 10)
				{
					for(m=0;m<50;m++)
					{
						if(j < 10)
						{
							pool_recv_one_command(&cse7766rx,&self_buf[24*j],24,CSE_7766_POOL);
							ret = Check_data_is_error(&self_buf[24*j],24);
							if(ret==1)
							{
								j++;
								break;
							}
						}
					}
				}
				len = pool_recv_one_command(&externalrx,exter_buf,50,EXTERNAL_POOL);
				if(len > 0)
				{
					break;
				}
			}
			for(m=0;m<j;m++)
			{
				if(Get_origin_pvi(&self_buf[24*m],24,&self_p_tmp[m],&self_v_tmp[m],&self_i_tmp[m])==1)
				{
					k++;
				}
			}
			if(k==0)
			{
				return -2;
			}
			for(m=0;m<10;m++)
			{
				self_p[i] += self_p_tmp[m];
				self_v[i] += self_p_tmp[m];
				self_p[i] += self_p_tmp[m];
			}
			self_p[i] /= k;
			self_v[i] /= k;
			self_p[i] /= k;

			self_p[i] = self_p[i]*self_adjust_coea_p+self_adjust_coeb_p;
			self_i[i] = self_i[i]*self_adjust_coea_i+self_adjust_coeb_i;

			Get_computer_external_pvi(exter_buf,len,&exter_p[i],&exter_v[i],&exter_i[i]);
			lcd_change_percent_info(100/load_cnt*i);
		}
		lcd_change_percent_info(100);
		Get_coe_a_b_r(exter_p,self_p,load_cnt,&ex_coea_p,&ex_coeb_p,&ex_coer_p);
		Get_coe_a_b_r(exter_i,self_i,load_cnt,&ex_coea_i,&ex_coeb_i,&ex_coer_i);

		if((ex_coer_p > 0.9) && (ex_coer_i > 0.9))
		{
			Send_result_to_c76(1,ex_coea_p,ex_coeb_p,ex_coea_i,ex_coeb_i,0);
			ret = 1;
		}
		else
		{
			Send_result_to_c76(0,ex_coea_p,ex_coeb_p,ex_coea_i,ex_coeb_i,0);
		}
		ret = 0;
	}

	lcd_show_coe_abr_info(ex_coea_p,ex_coeb_p,ex_coeb_i);

	if(ret==1)
	{
		return 1;
	}

	return 0;
}


int main()
{
	//double x[11] = {0.226856,0.444463,2.163232,2.377381,2.589446,4.273389,4.493904,
	//				6.365462,6.563147,8.465073,10.510610};
	//double y[11] = {0.26,0.47,2.24,2.46,2.68,4.4,4.64,6.58,6.67,8.71,10.84};

	double p=0,v=0,i=0;
	uint32_t len = 0;

	uint8_t data[500] = {0};
	uint8_t err_code[2] = {0};
	uint8_t ret = 0;
	int32_t ad_ret = 0;
	uint8_t exce_cnt = 0;
	uint8_t noadj_cnt = 0;
	uint8_t cycle_cnt = 0;

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

	//write_coe_from_flash();
	while (1)
    {
#if 1
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
				}
				else if(ad_ret == -1)
				{
					Debug_usart_write("self_adjust read 6530 failed\r\n",30,INFO_DEBUG);
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
			if(self_ad_ok_flag)
			{
				Debug_usart_write("Come to external_adjust mode\r\n",30,INFO_DEBUG);
				lcd_show_exter_now_adjust_info();
				Go_external_adjust();
				//operate_mode = 0;
			}
			else
			{
				lcd_show_local_noadjust_info();
			}
		}
		else if(operate_mode==0)
		{
			if(self_ad_ok_flag)
			{
				len = pool_recv_one_command(&cse7766rx,data,500,CSE_7766_POOL);
				//Debug_usart_write(data,len,DATA_SEND_RECV_DABUG);
				if(count_time_flag==1)
				{
					//len = pool_recv_one_command(&cse7766rx,data,500,CSE_7766_POOL);
					Debug_usart_write(data,len,DATA_SEND_RECV_DABUG);
					if(len > 0)
					{
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
						lcd_show_no_return_info();
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
