#ifndef __FITTING_METHOD_H
#define __FITTING_METHOD_H

#include "stm32f10x.h"

void Get_average_x_y(float *array_x,float *array_y,uint32_t len);
void Get_gather_all(float *array_x,float *array_y,uint32_t len);
float Get_coe_r(float *array_x,float *array_y,uint32_t len);
void Get_coe_a_b_r(float *array_x,float *array_y,uint32_t len,float *coea,float *coeb,float *coer);

#endif
