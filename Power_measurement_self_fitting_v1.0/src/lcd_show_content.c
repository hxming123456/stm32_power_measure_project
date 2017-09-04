#include "lcd_show_content.h"

void lcd_show_pvi_info(double p,double v,double i)
{
	uint8_t p_buf[10] = {0};
	uint8_t v_buf[10] = {0};
	uint8_t i_buf[10] = {0};

	iic_2864_clear_one(SHOW_PVI);
	iic_2864_Puts(0,0,(uint8_t *)"P:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"V:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"I:",&Font_11x18,(uint8_t)COLOR_WHITE);

	iic_2864_Puts(10,0,(uint8_t *)"W",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(10,1,(uint8_t *)"V",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(10,2,(uint8_t *)"A",&Font_11x18,(uint8_t)COLOR_WHITE);

	flodou_to_string(p,p_buf,Get_double_mantissa_len(&p),2);
	iic_2864_Puts(3,0,p_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	//Debug_usart_write(p_buf,str_len(p_buf),'Y');
	//Debug_usart_write("\t",2,'Y');

	flodou_to_string(v,v_buf,Get_double_mantissa_len(&v),2);
	iic_2864_Puts(3,1,v_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	//Debug_usart_write(v_buf,str_len(v_buf),'Y');
	//Debug_usart_write("\t",2,'Y');

	flodou_to_string(i,i_buf,Get_double_mantissa_len(&i),2);
	iic_2864_Puts(3,2,i_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	//Debug_usart_write(i_buf,str_len(i_buf),'Y');
	//Debug_usart_write("\r\n",2,'Y');
}

void lcd_show_local_noadjust_info(void)
{
	iic_2864_clear_one(INTER_NOADJ);
	iic_2864_Puts(0,0,(uint8_t *)"local:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"No Adjust",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_local_now_adjust_info(void)
{
	iic_2864_clear_one(INTER_ADJ);
	iic_2864_Puts(0,0,(uint8_t *)"Local:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Adjusting..",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"Per:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(10,2,(uint8_t *)"%",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(5,2,(uint8_t *)"0",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_exter_now_adjust_info(void)
{
	iic_2864_clear_one(EXTER_ADJ);
	iic_2864_Puts(0,0,(uint8_t *)"Exter:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Adjusting..",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"Per:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(10,2,(uint8_t *)"%",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(5,2,(uint8_t *)"0",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_noad_info(void)
{
	iic_2864_clear_one(NO_ADJUST);
	iic_2864_Puts(0,0,(uint8_t *)"CES7766:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"No adjust:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"Head:0xAA",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_err_code_info(uint8_t data)
{
	uint8_t buf[2] = {0};

	iic_2864_clear_one(EXCEPT);
	hex_to_str(buf,data);
	iic_2864_Puts(0,0,(uint8_t *)"CSE7766:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Exception:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"Head:0x",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(7,2,buf,&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_no_return_info(void)
{
	iic_2864_clear_one(NO_RETURN);
	iic_2864_Puts(0,0,(uint8_t *)"CSE7766:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"No Return..",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_self_read7766_error_info(void)
{
	iic_2864_clear_one(SELF_READ_7766_ERR);
	iic_2864_Puts(0,0,(uint8_t *)"Local adj:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Error:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"7766 Err..",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_self_read6530_error_info(void)
{
	iic_2864_clear_one(SELF_READ_7766_ERR);
	iic_2864_Puts(0,0,(uint8_t *)"Local adj:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Error:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"6530 Err..",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_recvc76_err_info(void)
{
	iic_2864_clear_one(EXTER_READ_C76_ERR);
	iic_2864_Puts(0,0,(uint8_t *)"exter adj:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Error:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"c76 Err..",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_exter_read7766_error_info()
{
	iic_2864_clear_one(EXTER_READ_7766_ERR);
	iic_2864_Puts(0,0,(uint8_t *)"exter adj:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Error:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"7766 Err..",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_exter_readc76pvi_error_info()
{
	iic_2864_clear_one(EXTER_READ_C76PVI_ERR);
	iic_2864_Puts(0,0,(uint8_t *)"exter adj:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Error:c76",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"pvi outime.",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_exter_c76pvi_data_error_info()
{
	iic_2864_clear_one(EXTER_READ_C76PVI_DATA_ERR);
	iic_2864_Puts(0,0,(uint8_t *)"exter adj:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Error:c76",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"Data Err.",&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_show_exter_stop_info()
{
	iic_2864_clear_one(EXTER_FAILED);
	iic_2864_Puts(0,0,(uint8_t *)"exter adj:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t *)"Error:c76",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t *)"Ad Failed",&Font_11x18,(uint8_t)COLOR_WHITE);
}

#if 1
void lcd_show_coe_abr_info(double a,double b,double r)
{
	uint32_t a_index = 4;
	uint32_t b_index = 4;
	uint8_t a_buf[10] = {0};
	uint8_t b_buf[10] = {0};
	uint8_t r_buf[10] = {0};

	iic_2864_clear_one(SHOW_ABR);

	if(a < 0)
	{
		a = -a;
		iic_2864_Puts(0,0,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		a_index++;
	}
	if(b < 0.00001)
	{
		b = -b;
		iic_2864_Puts(4,1,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		b_index++;
	}
	flodou_to_string(a,a_buf,Get_double_mantissa_len(&a),3);
	flodou_to_string(b,b_buf,Get_double_mantissa_len(&b),3);
	flodou_to_string(r,r_buf,Get_double_mantissa_len(&r),3);

	iic_2864_Puts(0,0,(uint8_t*)"P_A:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t*)"P_B:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t*)"P_R:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(a_index,0,a_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(b_index,1,b_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(4,2,r_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
}
#endif

void lcd_show_coe_allab_info(double p_a,double p_b,double i_a,double i_b)
{
	uint32_t pa_index = 2;
	uint32_t pb_index = 2;
	uint8_t pa_buf[10] = {0};
	uint8_t pb_buf[10] = {0};

	uint32_t ia_index = 7;
	uint32_t ib_index = 7;
	uint8_t ia_buf[10] = {0};
	uint8_t ib_buf[10] = {0};

	double pa_tmp = 0;
	double pb_tmp = 0;
	double ia_tmp = 0;
	double ib_tmp = 0;

	iic_2864_clear_one(SHOW_ALL_AB);

	if(p_a < 0)
	{
		p_a = -p_a;
		iic_2864_Puts(2,1,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		pa_index++;
	}
	if(p_b < 0.00001)
	{
		p_b = -p_b;
		iic_2864_Puts(2,2,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		pb_index++;
	}

	if(i_a < 0)
	{
		i_a = -i_a;
		iic_2864_Puts(7,1,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		ia_index++;
	}
	if(i_b < 0.00001)
	{
		i_b = -i_b;
		iic_2864_Puts(7,2,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		ib_index++;
	}

	pa_tmp = p_a*1000;
	pb_tmp = p_b*1000;
	ia_tmp = i_a*1000;
	ib_tmp = i_b*1000;

	flodou_to_string(pa_tmp,pa_buf,Get_double_mantissa_len(&pa_tmp),0);
	flodou_to_string(pb_tmp,pb_buf,Get_double_mantissa_len(&pb_tmp),0);
	flodou_to_string(ia_tmp,ia_buf,Get_double_mantissa_len(&ia_tmp),0);
	flodou_to_string(ib_tmp,ib_buf,Get_double_mantissa_len(&ib_tmp),0);

	iic_2864_Puts(2,0,(uint8_t*)"P",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(7,0,(uint8_t*)"I",&Font_11x18,(uint8_t)COLOR_WHITE);

	iic_2864_Puts(0,1,(uint8_t*)"a:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t*)"b:",&Font_11x18,(uint8_t)COLOR_WHITE);


	iic_2864_Puts(pa_index,1,pa_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(pb_index,2,pb_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(ia_index,1,ia_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(ib_index,2,ib_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_change_percent_info(uint32_t per)
{
	uint8_t buf[3] = {0};
	uint32_t per_tmp = per;


	if(per_tmp == 100)
	{
		iic_2864_Puts(5,2,(uint8_t *)"100",&Font_11x18,(uint8_t)COLOR_WHITE);
	}
	if(per_tmp < 10)
	{
		buf[0] = per+'0';
		iic_2864_Puts(5,2,buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	}
	else
	{
		buf[0] = per_tmp/10+'0';
		buf[1] = per_tmp%10+'0';
		iic_2864_Puts(5,2,buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	}
}

