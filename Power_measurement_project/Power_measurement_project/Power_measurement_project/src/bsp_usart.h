#ifndef __BSP_USART_H
#define __BSP_USART_H

#include "stm32f10x.h"
#include "Data_pool.h"

#define DATA_SEND_RECV_DABUG    'N'
#define INFO_DEBUG   			'Y'

extern uint8_t recv_data_flag;
extern uint32_t count_time_flag;
extern uint8_t uart4_recv_flag;

void CSE7766_usart_init();
void CSE7766_usart_write(uint8_t *data,uint32_t data_len);
void CSE7766_usart_gpio_init(void);
void CSE7766_usart_change_baud(uint32_t baud);
void CSE7766_usart_nvic_init(void);

void AC6530_usart_gpio_init(void);
void AC6530_usart_nvic_init(void);
void AC6530_usart_write(uint8_t *data,uint32_t data_len);
void AC6530_usart_init();

void Debug_usart_init();
void Debug_usart_gpio_init(void);
void Debug_usart_write(void *data,uint32_t data_len,uint8_t debug_type);
int Debug_usart_read(uint8_t *data);
void Debug_usart_nvic_init(void);

void Nextion_usart_init(void);
void Nextion_usart_gpio_init(void);
void Nextion_usart_nvic_init(void);
void Nextion_usart_write(uint8_t *data,uint32_t data_len);

uint32_t pool_recv_one_command(Datapool *pool_type,uint8_t *buf,uint32_t len,uint32_t type);

#endif
