#include<stc15f2k60s2.h>
#include"onewire.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff};

void select(uchar channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)|0x80;
			break;
		case 5:
			P2=(P2&0x1f)|0xa0;
			break;
		case 6:
			P2=(P2&0x1f)|0xc0;
			break;
		case 7:
			P2=(P2&0x1f)|0xe0;
			break;
		default:
			P2=(P2&0x1f);
			break;
	}
}

void delay(uint t)
{
	while(t--);
}

void display(uchar pos,uchar num)
{
	delay(200);
	P0=0xff;
	select(6);
	P0=0x01<<pos;
	select(7);
	P0=dig_code[num];
}

void init_sys()
{
	select(4);
	P0=0xff;
	select(5);
	P0=0x00;
	select(0);
}

uint get_temp()
{
	uchar LSB,MSB;
	uint temp=0;
	
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	
	LSB=Read_DS18B20();
	MSB=Read_DS18B20();
	init_ds18b20();
	temp=(MSB<<8)|LSB;
	temp=temp>>4;
	return temp;
}

void display_ds18b20()
{
	uint temp=get_temp();
	display(7,temp%10);
	display(6,temp/10);
	display(5,10);
	display(4,10);
	display(3,10);
	display(2,10);
	display(1,10);
	display(0,10);
}

void main()
{
	init_sys();
	while(1)
	{
		display_ds18b20();
	}
}