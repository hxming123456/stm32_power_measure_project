#include "iic_2864_operate.h"

typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_2864;

/* Private variable */
static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8] = {0};

static SSD1306_2864 ss2864;

uint8_t iic_2864_ssd1306_init(void)
{
	SSD1306_GPIO_INIT();

	/* A little delay */
#if 1
	GPIO_ResetBits(GPIOB,GPIO_Pin_6);
	iic_2864_Delay(100);
	GPIO_SetBits(GPIOB,GPIO_Pin_6);
	iic_2864_Delay(100);
#endif
	Debug_usart_write("connect ok\r\n",12,'Y');

#if 1
	SSD1306_WRITECOMMAND(0xae);

	SSD1306_WRITECOMMAND(0xD5);
	SSD1306_WRITECOMMAND(0x80);
	SSD1306_WRITECOMMAND(0xA8);
	SSD1306_WRITECOMMAND(0x3F);
	SSD1306_WRITECOMMAND(0xD3);
	SSD1306_WRITECOMMAND(0x00);
	SSD1306_WRITECOMMAND(0x40);
	SSD1306_WRITECOMMAND(0x8D);
	SSD1306_WRITECOMMAND(0x14);
	SSD1306_WRITECOMMAND(0xA1);
	SSD1306_WRITECOMMAND(0xC8);
	SSD1306_WRITECOMMAND(0xDA);
	SSD1306_WRITECOMMAND(0x12);
	SSD1306_WRITECOMMAND(0x81);
	SSD1306_WRITECOMMAND(0xCF);
	SSD1306_WRITECOMMAND(0xD9);
	SSD1306_WRITECOMMAND(0xF1);
	SSD1306_WRITECOMMAND(0xDB);
	SSD1306_WRITECOMMAND(0x40);
	SSD1306_WRITECOMMAND(0xA4);
	SSD1306_WRITECOMMAND(0xA6);

	iic_2864_fill(COLOR_BLACK);
	iic_2864_updatescreen();

	SSD1306_WRITECOMMAND(0xAF);
#endif
	ss2864.CurrentX = 0;
	ss2864.CurrentY = 0;
	ss2864.Initialized = 1;

	Debug_usart_write("2864 init ok\r\n",14,'Y');
	return 1;
}

uint32_t iic_2864_setxy(uint8_t x,uint8_t y)
{
	SSD1306_WRITECOMMAND(0xb0+y);
	SSD1306_WRITECOMMAND(((0xf0&x)>>4)|0x10);
	SSD1306_WRITECOMMAND(((0x0f&x)>>4)|0x01);

	return 0;
}

void iic_2864_updatescreen()
{
	uint8_t y;

	//SSD1306_WRITECOMMAND(0x00);
	//SSD1306_WRITECOMMAND(0x10);
	//SSD1306_WRITECOMMAND(0x40);

	for(y=0;y<SSD1306_HEIGHT/8;y++)
	{
		iic_2864_setxy(0,y);
		//for(x=0;x<SSD1306_WIDTH;x++)
		//{
		//	SSD1306_WRITEDATA(SSD1306_Buffer[(y+1)*x]);
		//}
		I2C_Send_more_data(&SSD1306_Buffer[SSD1306_WIDTH*y],SSD1306_WIDTH);
	}
}

void iic_2864_fill(uint8_t color)
{
	memset(SSD1306_Buffer, (color == COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
}

void iic_goto_xy(uint32_t x,uint32_t y)
{
	ss2864.CurrentX = x;
	ss2864.CurrentY = y;
}

uint8_t iic_putc(uint8_t ch,FontDef_t* Font,uint8_t color)
{
	uint32_t i,b,j;

	if (SSD1306_WIDTH <= (ss2864.CurrentX + Font->FontWidth) ||
		SSD1306_HEIGHT <= (ss2864.CurrentY + Font->FontHeight)
		)
	{
			/* Error */
			return 0;
	}

	for (i = 0; i < Font->FontHeight; i++)
	{
		b = Font->data[(ch - 32) * Font->FontHeight + i];
		for (j = 0; j < Font->FontWidth; j++)
		{
			if ((b << j) & 0x8000)
			{
				iic_2864_DrawPixel(ss2864.CurrentX + j, (ss2864.CurrentY + i),color);
			}
			else
			{
				iic_2864_DrawPixel(ss2864.CurrentX + j, (ss2864.CurrentY + i),!color);
			}
		}
	}

	ss2864.CurrentX += Font->FontWidth;

		/* Return character written */
	return ch;
}

uint8_t iic_2864_Puts(uint8_t x,uint8_t y,uint8_t* str, FontDef_t* Font, uint8_t color)
{
	iic_goto_xy(Font->FontWidth*(x)+1,Font->FontHeight*(y)+1);

	/* Write characters */
	while (*str) {
		/* Write character by character */
		if (iic_putc(*str, Font, color) != *str) {
			/* Return error */
			return *str;
		}

		/* Increase string pointer */
		str++;
	}

	iic_2864_updatescreen();

	/* Everything OK, zero should be returned */
	return *str;
}

void iic_2864_DrawPixel(uint32_t x, uint32_t y, uint8_t color)
{
	if (
		x >= SSD1306_WIDTH ||
		y >= SSD1306_HEIGHT
	) {
		/* Error */
		return;
	}

	/* Check if pixels are inverted */
	if (ss2864.Inverted) {
		color = !color;
	}

	/* Set color */
	if (color == COLOR_WHITE) {
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
	} else {
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}
}

void iic_2864_Delay(uint32_t time)
{
	uint32_t i,j;

	for(i=0;i<time;i++)
	{
		for(j=0;j<254;j++);
	}
}
