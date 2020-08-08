#include<stc15f2k60s2.h>
#include"ds1302.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf};
uchar time[7]={0x45,0x58,0x23,0x01,0x01,0x03,0x20};

void delay(uint t)
{
	while(t--);
}

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
		case 0:
			P2=(P2&0x1f);
			break;
	}
}

void init_sys()
{
	select(4);
	P0=0xff;
	select(5);
	P0=0x00;
	select(0);
}

void display(uchar pos,uchar value)
{
	delay(1000);
	P0=0xff;
	select(6);
	P0=0x01<<pos;
	select(7);
	P0=dig_code[value];
}

void init_ds1302()
{
	uchar address=0x80;
	uchar i;
	Write_Ds1302_Byte(0x8e,0x00); //关闭写保护
	for(i=0;i<7;i++) //轮流写入时间数据
	{
		Write_Ds1302_Byte(address,time[i]);
		address=address+2;
	}
	Write_Ds1302_Byte(0x8e,0x80);
}

void read_ds1302()
{
	uchar address=0x81;
	uchar i;
	for(i=0;i<7;i++)
	{
		time[i]=Read_Ds1302_Byte(address);
		address=address+2;
	}
}

void display_time()
{
	read_ds1302();
	display(0,time[2]/16);
	display(1,time[2]%16);
	display(2,10);
	display(3,time[1]/16);
	display(4,time[1]%16);
	display(5,10);
	display(6,time[0]/16);
	display(7,time[0]%16);
}

int main()
{
	init_sys();
	init_ds1302();
	while(1)
	{
		display_time();
	}
}