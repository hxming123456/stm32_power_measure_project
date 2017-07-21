#include "iic_2864_operate.h"

typedef struct {
	uint16_t CurrentX;
	uint16_t CurrentY;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_2864;

/* Private variable */
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

	iic_2864_updatescreen(0xFF);

	iic_2864_setxy(0,0);
	SSD1306_WRITECOMMAND(0xAF);
#endif

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

void iic_2864_updatescreen(uint8_t data)
{
	uint8_t x,y;

	SSD1306_WRITECOMMAND(0x00);
	SSD1306_WRITECOMMAND(0x10);
	SSD1306_WRITECOMMAND(0x40);

	for(y=0;y<SSD1306_HEIGHT/8;y++)
	{
		iic_2864_setxy(0,y);
		for(x=0;x<SSD1306_WIDTH;x++)
		{
			SSD1306_WRITEDATA(data);
		}
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
