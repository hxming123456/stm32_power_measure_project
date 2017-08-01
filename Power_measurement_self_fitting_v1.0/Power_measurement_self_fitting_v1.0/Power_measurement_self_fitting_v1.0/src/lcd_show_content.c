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

	flodou_to_string(v,v_buf,Get_double_mantissa_len(&v),2);
	iic_2864_Puts(3,1,v_buf,&Font_11x18,(uint8_t)COLOR_WHITE);

	flodou_to_string(i,i_buf,Get_double_mantissa_len(&i),2);
	iic_2864_Puts(3,2,i_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
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
	if(b < 0)
	{
		b = -b;
		iic_2864_Puts(4,1,(uint8_t*)"-",&Font_11x18,(uint8_t)COLOR_WHITE);
		b_index++;
	}
	flodou_to_string(a,a_buf,1,4);
	flodou_to_string(b,b_buf,1,4);
	flodou_to_string(r,r_buf,1,4);

	iic_2864_Puts(0,0,(uint8_t*)"P_A:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,1,(uint8_t*)"P_B:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(0,2,(uint8_t*)"P_R:",&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(a_index,0,a_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(b_index,1,b_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
	iic_2864_Puts(4,2,r_buf,&Font_11x18,(uint8_t)COLOR_WHITE);
}

void lcd_change_percent_info(uint32_t per)
{
	uint8_t buf[2] = {0};
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

