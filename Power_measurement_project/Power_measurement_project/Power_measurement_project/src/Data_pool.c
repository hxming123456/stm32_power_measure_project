#include "Data_pool.h"

uint8_t initDataPool(Datapool *obj)
{
	uint32_t i = 0;

	for(i=0;i<DATA_POOL_LEN;i++)
	{
		obj->buf[i] = 0;
	}
	obj->size = DATA_POOL_LEN;
	obj->stock = 0;
	obj->head = 0;
	obj->tail = 0;
	obj->avail = 0;

	return 0;
}

uint32_t updateRing(uint32_t len,uint32_t n)
{
	if((n+1)==len)
	{
		return 0;
	}
	else
	{
		return n+1;
	}
}

int32_t write_one_data_to_datapool(Datapool *obj,uint8_t data)
{
	if((obj->stock) < (obj->size))
	{
		obj->buf[obj->head] = data;
		obj->head = updateRing(obj->size,obj->head);
		obj->stock++;
		return 0;
	}
	else
	{
		return DATA_POOL_FULL;
	}
}

int32_t read_one_data_to_datapool(Datapool *obj,uint8_t *buf)
{
	uint8_t pos;

	if(obj->stock > 0)
	{
		pos = obj->tail;
		obj->tail = updateRing(obj->size,obj->tail);
		obj->stock--;
		*buf = obj->buf[pos];
		return 0;
	}
	else
	{
		return DATA_POOL_EMPTY;
	}
}

int32_t write_more_data_to_datapool(Datapool * obj, uint8_t *buf, uint32_t len)
{
	uint32_t i,ret;

	for(i=0;i<len;i++)
	{
		ret = write_one_data_to_datapool(obj,buf[i]);
		if(ret != 0)
		{
			len = i;
			return ret;
		}
	}

	return 0;
}

int32_t read_more_data_to_datapool(Datapool *obj,uint8_t *buf,uint32_t len)
{
	uint32_t i = 0;
	uint8_t ret;

	for(i=0;i<len;i++)
	{
		ret = read_one_data_to_datapool(obj,&buf[i]);
		if(ret != 0)
		{
			len = i;
			return len;
		}
	}

	return 0;
}

