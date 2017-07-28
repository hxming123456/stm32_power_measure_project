#ifndef __MAIN_H
#define __MAIN_H

extern uint8_t key_status;
extern uint8_t key_backup;
extern uint32_t down_time;
extern uint32_t up_time;

#define RELAY_CONTR_KEY_READ  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_1)
#define MORE_RELAY_INIT(a,b)		  more_relay_pin_init(a,b)

#define P_T0_CONTROL_TYPE				0
#define V_T1_CONTROL_TYPE				1
#define I_T2_CONTROL_TYPE				2
#define STA_CONTROL_TYPE				3

#define CHO_HIGH		GPIO_SetBits(GPIOA,GPIO_Pin_8);
#define CH1_HIGH		GPIO_SetBits(GPIOC,GPIO_Pin_7);
#define CH2_HIGH		GPIO_SetBits(GPIOC,GPIO_Pin_6);
#define CH3_HIGH		GPIO_SetBits(GPIOB,GPIO_Pin_15);
#define CH4_HIGH		GPIO_SetBits(GPIOB,GPIO_Pin_14);
#define CH5_HIGH		GPIO_SetBits(GPIOB,GPIO_Pin_13);
#define CH6_HIGH		GPIO_SetBits(GPIOB,GPIO_Pin_12);
#define CH7_HIGH		GPIO_SetBits(GPIOB,GPIO_Pin_1);
#define CH8_HIGH		GPIO_SetBits(GPIOB,GPIO_Pin_0);
#define CH9_HIGH		GPIO_SetBits(GPIOC,GPIO_Pin_5);
#define CH10_HIGH		GPIO_SetBits(GPIOC,GPIO_Pin_4);
#define CH11_HIGH		GPIO_SetBits(GPIOA,GPIO_Pin_7);
#define CH12_HIGH		GPIO_SetBits(GPIOA,GPIO_Pin_6);
#define CH13_HIGH		GPIO_SetBits(GPIOA,GPIO_Pin_5);
#define CH14_HIGH		GPIO_SetBits(GPIOA,GPIO_Pin_4);
#define CH15_HIGH		GPIO_SetBits(GPIOA,GPIO_Pin_1);

#define CHO_LOW			GPIO_ResetBits(GPIOA,GPIO_Pin_8);
#define CH1_LOW			GPIO_ResetBits(GPIOC,GPIO_Pin_7);
#define CH2_LOW			GPIO_ResetBits(GPIOC,GPIO_Pin_6);
#define CH3_LOW			GPIO_ResetBits(GPIOB,GPIO_Pin_15);
#define CH4_LOW			GPIO_ResetBits(GPIOB,GPIO_Pin_14);
#define CH5_LOW			GPIO_ResetBits(GPIOB,GPIO_Pin_13);
#define CH6_LOW			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
#define CH7_LOW			GPIO_ResetBits(GPIOB,GPIO_Pin_1);
#define CH8_LOW			GPIO_ResetBits(GPIOB,GPIO_Pin_0);
#define CH9_LOW			GPIO_ResetBits(GPIOC,GPIO_Pin_5);
#define CH10_LOW		GPIO_ResetBits(GPIOC,GPIO_Pin_4);
#define CH11_LOW		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
#define CH12_LOW		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
#define CH13_LOW		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
#define CH14_LOW		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
#define CH15_LOW		GPIO_ResetBits(GPIOA,GPIO_Pin_1);

#define CH16_HIGH		GPIO_SetBits(GPIOB,GPIO_Pin_2);
#define CH16_LOW		GPIO_ResetBits(GPIOB,GPIO_Pin_2);

#endif
