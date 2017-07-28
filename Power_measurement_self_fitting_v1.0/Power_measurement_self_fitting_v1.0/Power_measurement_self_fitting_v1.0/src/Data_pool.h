#ifndef __DATA_POOL_H
#define __DATA_POOL_H

#include "stm32f10x.h"

#define DATA_POOL_LEN 	96
#define DATA_POOL_FULL  -1
#define DATA_POOL_EMPTY -2

typedef struct DataPool{
	uint8_t buf[DATA_POOL_LEN];
	int size;
	int stock;
	int head;
	int tail;
	int avail;
}Datapool;

Datapool cse7766rx;
Datapool ac6530rx;
Datapool externalrx;

#define CSE_7766_POOL	0
#define AC_6530_POOL	1
#define EXTERNAL_POOL	2

uint8_t initDataPool(Datapool *obj);
uint32_t updateRing(uint32_t len,uint32_t n);
int32_t write_one_data_to_datapool(Datapool *obj,uint8_t data);
int32_t read_one_data_to_datapool(Datapool *obj,uint8_t *buf);
int32_t write_more_data_to_datapool(Datapool * obj, uint8_t *buf, uint32_t len);
int32_t read_more_data_to_datapool(Datapool *obj,uint8_t *buf,uint32_t len);


#endif
