#ifndef __BSP_TIM_H
#define __BSP_TIM_H

#include "stm32f10x.h"
#include "bsp_usart.h"
#include "main.h"

#define MAIN_COUNT_TIME_VAL	500
#define EXTERNAL_COUNT_TIME_VAL 100

extern uint32_t recv_timeout_start;
extern uint32_t recv_timeout_end;
extern uint32_t recv_timeout;

extern uint32_t pool_wait_time;
extern uint32_t external_wait_time;
void tim_init(void);
void delay_ms(int32_t timecnt);
void delay_s(int32_t scnt);
void change_light_status(uint8_t status);

#endif
