#ifndef __BSP_INTERNAL_FLASH_H
#define __BSP_INTERNAL_FLASH_H

#include "stm32f10x.h"

void flash_read_more_data(uint32_t address,uint16_t *data,uint32_t len);
uint16_t flash_read_one_data(uint32_t address);
uint32_t flash_read32_data(uint32_t address);
uint32_t flash_write_more_data(uint32_t address,uint16_t *data,uint32_t len);

#endif
