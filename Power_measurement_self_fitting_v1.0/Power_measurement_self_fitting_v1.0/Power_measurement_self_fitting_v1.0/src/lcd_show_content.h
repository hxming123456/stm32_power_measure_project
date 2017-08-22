#ifndef __LCD_SHOW_CONTENT_H
#define __LCD_SHOW_CONTENT_H

#include "stm32f10x.h"
#include "iic_2864_operate.h"
#include "Clib.h"
#include "main.h"

enum{
	EXCEPT=1,
	CYCLE_ERROR,
	NO_ADJUST,
	SHOW_PVI,
	EXTER_ADJ,
	INTER_ADJ,
	INTER_NOADJ,
	NO_RETURN,
	SHOW_ABR,
	SELF_READ_7766_ERR,
	EXTER_READ_C76_ERR,
	EXTER_READ_7766_ERR,
	EXTER_READ_C76PVI_ERR,
	EXTER_READ_C76PVI_DATA_ERR,
	KEY_CHANGE,
	SHOW_ALL_AB,
	ALL_BLACK,
}status;

void lcd_show_pvi_info(double p,double v,double i);
void lcd_show_local_noadjust_info(void);
void lcd_show_local_now_adjust_info(void);
void lcd_show_exter_now_adjust_info(void);
void lcd_change_percent_info(uint32_t per);
void lcd_show_noad_info(void);
void lcd_show_err_code_info(uint8_t data);
void lcd_show_no_return_info(void);
void lcd_show_coe_abr_info(double a,double b,double r);
void lcd_show_coe_allab_info(double p_a,double p_b,double i_a,double i_b);
void lcd_show_recvc76_err_info(void);
void lcd_show_exter_read7766_error_info();
void lcd_show_exter_readc76pvi_error_info();
void lcd_show_self_read7766_error_info();
void lcd_show_self_read6530_error_info();
void lcd_show_exter_c76pvi_data_error_info();

#endif
