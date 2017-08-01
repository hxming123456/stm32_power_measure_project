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

#endif
