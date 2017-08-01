#ifndef __WRRE_COE_FROM_FLASH_H
#define __WRRE_COE_FROM_FLASH_H

#include "stm32f10x.h"
#include "main.h"
#include "bsp_usart.h"
#include "bsp_internal_flash.h"

#define FLASH_START_ADDR	0x8000000+2048*200

uint32_t Get_double_mantissa_len(double *data);
void make_coe_to_data(uint16_t *data);
void pars_coe_from_data(uint16_t *data);
uint32_t write_coe_from_flash(void);
uint8_t read_coe_from_flash(void);

#endif
