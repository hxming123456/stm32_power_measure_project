#include "WrRe_coe_from_flash.h"

uint32_t Get_double_mantissa_len(double *data)
{
	double len_2 = 10.0;
	double len_3 = 100.0;
	double len_4 = 1000.0;

	if(*data < len_2)
			return 1;
	else if(*data > len_2 && *data < len_3)
			return 2;
	else if(*data > len_3 && *data < len_4)
			return 3;
	else
			return 4;
}

void make_coe_to_data(uint16_t *data)
{
	uint16_t tmp_int = 0;
	uint16_t tmp_dec = 0;
	double tmp_coea_p = self_adjust_coea_pl;
	double tmp_coeb_p = self_adjust_coeb_pl;
	double tmp_coer_p = self_adjust_coer_pl;

	double tmp_coea_i = self_adjust_coea_i;
	double tmp_coeb_i = self_adjust_coeb_i;
	double tmp_coer_i = self_adjust_coer_i;

	double tmp_coea_ph = self_adjust_coea_ph;
	double tmp_coeb_ph = self_adjust_coeb_ph;
	double tmp_coer_ph = self_adjust_coer_ph;

	double tmp_com_v = self_adjust_comv;

	double tmp_coe = 0;

	data[0] = 'Y';

	if(tmp_coea_p < 0)
	{
		tmp_coea_p = -tmp_coea_p;
		data[1] = 1;
	}
	else
	{
		data[1] = 0;
	}
	tmp_coe = tmp_coea_p-(uint32_t)tmp_coea_p;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coea_p;
	data[2] = tmp_int;
	data[3] = tmp_dec;

	if(tmp_coeb_p < 0)
	{
		tmp_coeb_p = -tmp_coeb_p;
		data[4] = 1;
	}
	else
	{
		data[4] = 0;
	}
	tmp_coe = tmp_coeb_p-(uint32_t)tmp_coeb_p;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coeb_p;
	data[5] = tmp_int;
	data[6] = tmp_dec;


	if(tmp_coer_p < 0)
	{
		tmp_coer_p = -tmp_coer_p;
		data[7] = 1;
	}
	else
	{
		data[7] = 0;
	}
	tmp_coe = tmp_coer_p-(uint32_t)tmp_coer_p;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coer_p;
	data[8] = tmp_int;
	data[9] = tmp_dec;

	if(tmp_coea_i < 0)
	{
		tmp_coea_i = -tmp_coea_i;
		data[10] = 1;
	}
	else
	{
		data[10] = 0;
	}
	tmp_coe = tmp_coea_i-(uint32_t)tmp_coea_i;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coea_i;
	data[11] = tmp_int;
	data[12] = tmp_dec;

	if(tmp_coeb_i < 0)
	{
		tmp_coeb_i = -tmp_coeb_i;
		data[13] = 1;
	}
	else
	{
		data[13] = 0;
	}
	tmp_coe = tmp_coeb_i-(uint32_t)tmp_coeb_i;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coeb_i;
	data[14] = tmp_int;
	data[15] = tmp_dec;

	if(tmp_coer_i < 0)
	{
		tmp_coer_i = -tmp_coer_i;
		data[16] = 1;
	}
	else
	{
		data[16] = 0;
	}
	tmp_coe = tmp_coer_i-(uint32_t)tmp_coer_i;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coer_i;
	data[17] = tmp_int;
	data[18] = tmp_dec;

	if(tmp_com_v < 0)
	{
		tmp_com_v = -tmp_com_v;
		data[19] = 1;
	}
	else
	{
		data[19] = 0;
	}
	tmp_coe = tmp_com_v-(uint32_t)tmp_com_v;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_com_v;
	data[20] = tmp_int;
	data[21] = tmp_dec;

	if(tmp_coea_ph < 0)
	{
		tmp_coea_ph = -tmp_coea_ph;
		data[22] = 1;
	}
	else
	{
		data[22] = 0;
	}
	tmp_coe = tmp_coea_ph-(uint32_t)tmp_coea_ph;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coea_ph;
	data[23] = tmp_int;
	data[24] = tmp_dec;

	if(tmp_coeb_ph < 0)
	{
		tmp_coeb_ph = -tmp_coeb_ph;
		data[25] = 1;
	}
	else
	{
		data[25] = 0;
	}
	tmp_coe = tmp_coeb_ph-(uint32_t)tmp_coeb_ph;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coeb_ph;
	data[26] = tmp_int;
	data[27] = tmp_dec;

	if(tmp_coer_ph < 0)
	{
		tmp_coer_ph = -tmp_coer_ph;
		data[28] = 1;
	}
	else
	{
		data[28] = 0;
	}
	tmp_coe = tmp_coer_ph-(uint32_t)tmp_coer_ph;
	tmp_dec = (tmp_coe*10000);
	tmp_int = (uint16_t)tmp_coer_ph;
	data[29] = tmp_int;
	data[30] = tmp_dec;
}

void pars_coe_from_data(uint16_t *data)
{
	double tmp_int=0;
	double tmp_dec=0;

	tmp_int = (double)data[1];
	tmp_dec = (double)data[2]/10000;
	if(data[0]==0)
	{
		self_adjust_coea_pl = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coea_pl = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[4];
	tmp_dec = (double)data[5]/10000;
	if(data[3]==0)
	{
		self_adjust_coeb_pl = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coeb_pl = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[7];
	tmp_dec = (double)data[8]/10000;
	if(data[6]==0)
	{
		self_adjust_coer_pl = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coer_pl = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[10];
	tmp_dec = (double)data[11]/10000;
	if(data[9]==0)
	{
		self_adjust_coea_i = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coea_i = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[13];
	tmp_dec = (double)data[14]/10000;
	if(data[12]==0)
	{
		self_adjust_coeb_i = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coeb_i = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[16];
	tmp_dec = (double)data[17]/10000;
	if(data[15]==0)
	{
		self_adjust_coer_i = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coer_i = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[19];
	tmp_dec = (double)data[20]/10000;
	if(data[18]==0)
	{
		self_adjust_comv = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_comv = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[22];
	tmp_dec = (double)data[23]/10000;
	if(data[21]==0)
	{
		self_adjust_coea_ph = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coea_ph = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[25];
	tmp_dec = (double)data[26]/10000;
	if(data[24]==0)
	{
		self_adjust_coeb_ph = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coeb_ph = -(tmp_int+tmp_dec);
	}

	tmp_int = (double)data[28];
	tmp_dec = (double)data[29]/10000;
	if(data[27]==0)
	{
		self_adjust_coer_ph = tmp_int+tmp_dec;
	}
	else
	{
		self_adjust_coer_ph = -(tmp_int+tmp_dec);
	}
}

uint32_t write_coe_from_flash(void)
{
	uint16_t data[31] = {0};
	uint32_t ret = 0;

	make_coe_to_data(data);
	ret = flash_write_more_data(FLASH_START_ADDR,data,31);
	if(ret)
	{
		Debug_usart_write("write flash ok\r\n",16,INFO_DEBUG);
	}

	return ret;
}

uint8_t read_coe_from_flash(void)
{
	uint16_t data[31] = {0};

	flash_read_more_data(FLASH_START_ADDR,data,31);

	if(data[0]=='Y')
	{
		pars_coe_from_data(&data[1]);
		return 1;
	}
	else
	{
		return 0;
	}
}
