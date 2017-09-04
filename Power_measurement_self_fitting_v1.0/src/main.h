#ifndef __MAIN_H
#define __MAIN_H

extern uint8_t key_status;
extern uint8_t key_backup;
extern uint32_t down_time;
extern uint32_t up_time;
extern uint32_t external_wait_flag;

enum
{
	EXTERNAL_START=1,
	EXTERNAL_READ_NOW,
}External_sta;

#define RELAY_CONTR_KEY_READ  		  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
#define SELF_ADJUST_KEY_READ		  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)
#define EXTERNAL_KEY_READ			  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3)

#define MORE_RELAY_INIT(a,b)		  more_relay_pin_init(a,b)

#define SELF_ADJUST_MODE				1
#define EXTERNAL_ADJUST_MODE			2

#define P_T0_CONTROL_TYPE				0
#define V_T1_CONTROL_TYPE				1
#define I_T2_CONTROL_TYPE				2
#define STA_CONTROL_TYPE				3

extern double self_adjust_coea_pl;
extern double self_adjust_coeb_pl;
extern double self_adjust_coer_pl;

extern double self_adjust_coea_ph;
extern double self_adjust_coeb_ph;
extern double self_adjust_coer_ph;

extern double self_adjust_coea_i;
extern double self_adjust_coeb_i;
extern double self_adjust_coer_i;

extern double self_adjust_coea_il;
extern double self_adjust_coeb_il;
extern double self_adjust_coer_il;

extern double self_adjust_coea_pl_110;
extern double self_adjust_coeb_pl_110;
extern double self_adjust_coer_pl_110;

extern double self_adjust_coea_ph_110;
extern double self_adjust_coeb_ph_110;
extern double self_adjust_coer_ph_110;

extern double self_adjust_coea_i_110;
extern double self_adjust_coeb_i_110;
extern double self_adjust_coer_i_110;

extern double self_adjust_coea_il_110;
extern double self_adjust_coeb_il_110;
extern double self_adjust_coer_il_110;

extern double external_adjust_coea_p;
extern double external_adjust_coeb_p;
extern double external_adjust_coer_p;

extern double external_adjust_coea_i;
extern double external_adjust_coeb_i;
extern double external_adjust_coer_i;

extern double self_adjust_comv;
extern double self_adjust_comv_110;
extern double external_adjust_comv;

uint32_t Get_double_mantissa_len(double *data);
uint32_t write_coe_from_flash(uint8_t v_type);

#endif
