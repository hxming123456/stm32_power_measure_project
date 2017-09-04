#ifndef __FITTING_METHOD_H
#define __FITTING_METHOD_H

#include "stm32f10x.h"

void Get_average_x_y(double *array_x,double *array_y,uint32_t len);
void Get_gather_all(double *array_x,double *array_y,uint32_t len);
double Get_coe_r(double *array_x,double *array_y,uint32_t len);
void Get_coe_a_b_r(double *array_x,double *array_y,uint32_t len,double *coea,double *coeb,double *coer);

#endif
