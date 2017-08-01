#include "bsp_internal_flash.h"

#define  FLASH_SIZE  512

#if FLASH_SIZE<256
	#define	SECTOR_SIZE			1024
#else
	#define SECTOR_SIZE			2048
#endif

void flash_read_more_data(uint32_t address,uint16_t *data,uint32_t len)
{
	uint16_t i;

	for(i=0;i<len;i++)
	{
		data[i] = flash_read_one_data(address+i*2);
	}
}

uint16_t flash_read_one_data(uint32_t address)
{
	return *(__IO uint16_t*)address;
}

uint32_t flash_read32_data(uint32_t address)
{
	uint32_t temp1,temp2;

	temp1 = *(__IO uint16_t *)address;
	temp2 = *(__IO uint16_t *)(address+2);

	return (temp2<<16)+temp1;
}


uint32_t flash_write_more_data(uint32_t address,uint16_t *data,uint32_t len)
{
	uint32_t offset_address = address - FLASH_BASE;
	uint32_t sector_address = offset_address/SECTOR_SIZE;
	uint32_t sector_start_address = sector_address*SECTOR_SIZE+FLASH_BASE;

	uint16_t index;

	if(address<FLASH_BASE || ((address+len*2) >= (FLASH_BASE+1024*FLASH_SIZE)))
	{
		return 0;
	}

	FLASH_Unlock();
	FLASH_ErasePage(sector_start_address);
	for(index=0;index<len;index++)
	{
		FLASH_ProgramHalfWord(address+index*2,data[index]);
	}

	FLASH_Lock();
	return 1;
}
