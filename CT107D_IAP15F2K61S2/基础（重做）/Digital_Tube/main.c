#include<reg52.h>
typedef unsigned int uint;
typedef unsigned char uchar;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f};

void select(uchar channel)
{
	switch(channel)
	{
		case 4: P2=(P2&0X1F)|0X80;break;
		case 5: P2=(P2&0X1F)|0XA0;break;
		case 6: P2=(P2&0X1F)|0XC0;break;
		case 7: P2=(P2&0X1F)|0XE0;break;
	}
}
void init()
{
	select(5);
	P0=0X00;
}

void delay_tube(uint t)
{
	while(t--);
}

void display(uchar channel,uchar value)
{
		select(6);
		P0=0X01<<channel;
		select(7);
		P0=dig_code[value];
}


void main()
{
	init();
	while(1)
	{
		display(0,0);
		delay_tube(200);
		P0=0X00;
		display(1,1);
		delay_tube(200);
		P0=0X00;
	}
}