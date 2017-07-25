#ifndef __MAIN_H
#define __MAIN_H

extern uint8_t key_status;
extern uint8_t key_backup;
extern uint32_t down_time;
extern uint32_t up_time;

#define RELAY_CONTR_KEY_READ  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_2)

#define P_T0_CONTROL_TYPE				0
#define V_T1_CONTROL_TYPE				1
#define I_T2_CONTROL_TYPE				2
#define STA_CONTROL_TYPE				3

#endif
