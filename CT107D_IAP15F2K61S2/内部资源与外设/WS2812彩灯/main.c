#include <stc15f2k60s2.h>
#include <intrins.h>
#define NOP {_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
sbit DIN=P1^0;
void init_sys()
{
	P2=(P2&0x1f)|0x80;
	P0=0xff;
	P2=(P2&0x1f)|0xa0;
	P0=0x00;
	P2=(P2&0x1f);
}

void ws2812_init()
{
	uchar i;
	DIN=0;
	for(i=0;i<=200;i++)
	{
		NOP;
	}
}

void ws2812_write_low()
{
	DIN=1;
	NOP;
	DIN=0;
	NOP;
	NOP;
	NOP;
	NOP;
}

void ws2812_write_high()
{
	DIN=1;
	NOP;
	NOP;
	NOP;
	NOP;
	DIN=0;
	NOP;
}

void ws2812_write_byte(uchar dat)
{
	uchar i;
	for(i=0;i<8;i++)
	{
		if(dat&0x80)
		{
			ws2812_write_high();
		}
		else
		{
			ws2812_write_low();
		}
		dat<<=1;
	}
}

void ws2812_write_rgb(uchar r,uchar g,uchar b)
{
	ws2812_write_byte(g);
	ws2812_write_byte(r);
	ws2812_write_byte(b);
}

void ws2812_write_24bits(ulong dat)
{
	ws2812_write_byte(dat>>8); //write_r
	ws2812_write_byte(dat>>16); //write_g
	ws2812_write_byte(dat); //write_b
}


void main()
{
	init_sys();
	ws2812_init();
	ws2812_write_24bits(0x000000);
	ws2812_write_24bits(0xff0000);
	ws2812_write_24bits(0x00ff00);
	ws2812_write_24bits(0x0000ff);
	ws2812_write_24bits(0xffff00);
	ws2812_write_24bits(0xff00ff);
	ws2812_write_24bits(0x00ffff);
	ws2812_write_24bits(0xffffff);
	while(1)
	{
		 
	}
}