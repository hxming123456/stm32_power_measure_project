#include "Fitting_method.h"

double gather_xy = 0.0f;
double gather_x = 0.0f;
double gather_y = 0.0f;
double gather_x2 = 0.0f;
double gather_y2 = 0.0f;
double gather_x_2 = 0.0f;
double average_x = 0.0f;
double average_y = 0.0f;

double coe_a = 0.0f;
double coe_b = 0.0f;
double coe_r = 0.0f;


void Get_average_x_y(double *array_x,double *array_y,uint32_t len)
{
	uint32_t i = 0;
	double sum_x=0.0f,sum_y=0.0f;
	
	for(i=0;i<len;i++)
	{
		sum_x += array_x[i];
		sum_y += array_y[i];
	}
	
	average_x = sum_x/len;
	average_y = sum_y/len;
}

void Get_gather_all(double *array_x,double *array_y,uint32_t len)
{
	uint32_t i = 0;
	
	for(i=0;i<len;i++)
	{
		gather_xy += (array_x[i]*array_y[i]);
		gather_x += array_x[i];
		gather_y += array_y[i];
		gather_x2 += (array_x[i]*array_x[i]);
		gather_y2 += (array_y[i]*array_y[i]);
	}
	
	gather_x_2 = gather_x*gather_x;
}

double Get_coe_r(double *array_x,double *array_y,uint32_t len)
{
	uint32_t i = 0;
	double gather_x_X=0,gather_y_Y=0;
	double gather_x_X_2=0,gather_y_Y_2=0;
	double ret = 0;

	for(i=0;i<len;i++)
	{
		gather_x_X += (array_x[i] - average_x);
		gather_y_Y += (array_y[i] - average_y);
		gather_x_X_2 += (gather_x_X*gather_x_X);
		gather_y_Y_2 += (gather_y_Y*gather_y_Y);
	}


	if((gather_x_X==0) || (gather_y_Y==0))
	{
		return 1;
	}
	ret = ((gather_x_X*gather_x_X)*(gather_y_Y*gather_y_Y))/(gather_x_X_2*gather_y_Y_2);

	return ret;
}

void Get_coe_a_b_r(double *array_x,double *array_y,uint32_t len,double *coea,double *coeb,double *coer)
{
	Get_gather_all(array_x,array_y,len);
	
	*coea = (len*gather_xy-gather_x*gather_y)/(len*gather_x2-gather_x_2);
	Get_average_x_y(array_x,array_y,len);
	*coeb = average_y-(average_x*(*coea));
	*coer = (gather_xy*gather_xy)/(gather_x2*gather_y2);
	//*coer = Get_coe_r(array_x,array_y,len);
}
