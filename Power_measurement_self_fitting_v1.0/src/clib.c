#include "clib.h"

uint8_t* str_str(uint8_t *s1, uint8_t *s2)
{
	int n;
	if (*s2 != 0)
	{
		while (*s1 != 0)
		{
			for (n = 0; *(s1 + n) == *(s2 + n); n++)
			{
				if (!*(s2 + n + 1))
				{
					return s1;
				}
			}
			s1++;
		}
		return 0;
	}
	else
	{
		return s1;
	}
}

uint8_t *str_chr(uint8_t *s, uint8_t c)
{
	//uint8_t *p = s;

	 while (*s && *s != c)
	 {
		 s++;
	 }

	 if (*s == c)
	 {
		 return s;
	 }

    return 0;
 }

void clear_string_buf(uint8_t *buf, uint32_t len)
{
	while (len--)
	{
		*buf = 0;
		buf++;
	}
}

uint32_t str_len(uint8_t *buf)
{
	uint32_t buf_len = 0;

	if (buf == 0)
	{
		return 0;
	}

	while (*buf != '\0')
	{
		buf_len++;
		buf++;
	}

	return buf_len;
}

void str_cat(uint8_t *dest,uint32_t des_len,uint8_t *str,uint32_t str_len)
{
	uint32_t i = 0;

	for(i=0;i<str_len;i++)
	{
		dest[des_len+i] = str[i];
	}
}

uint32_t Strncpy(uint8_t *dest, uint8_t *src, uint32_t n)
{
	uint8_t *strDest = dest;

	if (dest == 0 || src == 0)
	{
		return 0;
	}

	while (*src != '\0' && n != 0)
	{
		*strDest = *src;
		strDest++;
		src++;
		n--;
	}

	return 1;
}

int32_t Atoi(uint8_t *str)
{
	uint32_t bMinus = 0;
	int32_t result = 0;

	if (!str)
	{
		return -1;
	}

	if (('0'>*str || *str>'9') && (*str == '+' || *str == '-'))
	{
		if (*str == '-')
		{
			bMinus = 1;
		}
		str++;
	}
	while (*str != '\0')
	{
		if ('0' > *str || *str > '9')
		{
			break;
		}
		else
		{
			result = result * 10 + (*str++ - '0');
		}
	}

	if (bMinus)
	{
		return -result;
	}
	else
	{
		return result;
	}

}

void Itoa(uint32_t i, uint8_t *string)
{
	uint32_t power = 0, j = 0;

	j = i;

	for (power = 1; j > 10; j /= 10)
	{
		power *= 10;
	}

	for (; power>0; power /= 10)
	{
		*string++ = '0' + i / power;
		i %= power;
	}

	//*string = '\0';

}

#if 1
void flodou_to_string(double number,uint8_t *str,uint8_t int_len,uint8_t dec_len)
{
	uint8_t i;
	uint8_t cnt = 0;
	uint32_t temp = number / 1;
	double t2 = 0.0;

	for (i = 1; i <= int_len; i++)
	{
		if (temp == 0)
		{
			str[int_len - i] = '0';
		}
		else
		{
			str[int_len - i] = (temp % 10) + '0';
		}
		temp = temp / 10;
	}
	if(dec_len > 0)
	{
		*(str + int_len) = '.';
		temp = 0;
		t2 = number;
		for (i = 1; i <= dec_len; i++)
		{
			temp = t2 * 10;
			str[int_len + i] = (temp % 10)+'0';
			t2 = t2 * 10;
		}
		*(str + int_len + dec_len + 1) = '\0';
	}
}
#endif

uint32_t find_the_most_element(uint32_t *data, uint32_t len)
{
    uint32_t position_the_most_ele = 0;
    uint32_t num_the_most_ele = 0;
    uint32_t num = 1;
    uint32_t i,j;

    for(i = 0; i < len - 1; i++)
    {

	for(j = i + 1; j < len; j++)
	{
	    if( *(data + i) == *(data + j) )
	    {
		num++;
	    }
	}

	if(num > num_the_most_ele)
	{
	    num_the_most_ele = num;
	    position_the_most_ele = i;
	}

	num = 1;

    }

    return *(data + position_the_most_ele);
}

void hex_to_str(uint8_t *str,uint8_t hex)
{
	uint8_t tmp=0x00;

	tmp = hex >> 4;
	if(tmp >= 0 && tmp <= 9)
	{
		str[0] = tmp + '0';
	}
	else if(tmp >= 10)
	{
		str[0] = 'A' + tmp - 10;
	}

	tmp = hex & 0x0F;
	if(tmp >= 0 && tmp <= 9)
	{
		str[1] = tmp + '0';
	}
	else if(tmp >= 10)
	{
		str[1] = 'A' + tmp - 10;
	}
}

void more_hex_to_str(uint8_t *str,uint8_t *hex,uint8_t len)
{
	uint8_t tmp=0x00;
	uint8_t i = 0;
	uint16_t j = 0;

	for(i=0;i<len;i++)
	{
		tmp = hex[i] >> 4;
		if(tmp >= 0 && tmp <= 9)
		{
			str[j++] = tmp + '0';
		}
		else if(tmp >= 10)
		{
			str[j++] = 'A' + tmp - 10;
		}

		tmp = hex[i] & 0x0F;
		if(tmp >= 0 && tmp <= 9)
		{
			str[j++] = tmp + '0';
		}
		else if(tmp >= 10)
		{
			str[j++] = 'A' + tmp - 10;
		}
	}
}

uint8_t str_to_hex(uint8_t str1,uint8_t str2)
{
	uint8_t tmp=0xFF;
	if((str1 >= '0')&&(str1<='9'))
	{
		tmp = str1 - '0';
	}
	else if((str1 >= 'a')&&(str1<='f'))
	{
		tmp = 10 + (str1 - 'a');
	}
	else if((str1 >= 'A')&&(str1<='F'))
	{
		tmp = 10 + (str1 - 'A');
	}
	tmp <<= 4;
	if((str2 >= '0')&&(str2<='9'))
	{
		tmp |= (str2 - '0');
	}
	else if((str2 >= 'a')&&(str2<='f'))
	{
		tmp |= (10 + (str2 - 'a'));
	}
	else if((str2 >= 'A')&&(str2<='F'))
	{
			tmp |= (10 + (str2 - 'A'));
	}
	return tmp;
}

uint32_t string_to_flodou(double *number,uint8_t *str,uint32_t len)
{
	uint32_t i=0,j=0,k=0;
	double tmp = 1;
	double int_num = 0;
	double dec_num = 0;

	for(i=0;i<len;i++)
	{
		if(str[i]=='.')
		{
			break;
		}
	}

	for(j=0;j<i;j++)
	{
		int_num += (str[j]-'0');
		int_num *= 10;
	}
	int_num /= 10;

#if 1
	k = len - i;
	if(k == 0)
	{
		*number = int_num;
		return 1;
	}

	tmp = 1;
	for(j=i+1;j<len;j++)
	{
		dec_num += (str[j]-'0');
		dec_num *= 10;
	}

	dec_num /= 10;
	for(i=0;i<k-1;i++)
	{
		tmp *= 10;
	}

	dec_num /= tmp;

	*number = int_num+dec_num;
#endif
	return 1;
}







