#include<reg52.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sbit HC138_A=P2^5;
sbit HC138_B=P2^6;
sbit HC138_C=P2^7;

void Delay1000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void Initsys()
{
	HC138_C=1;
	HC138_B=0;
	HC138_A=1;
	P0=0X00;
}

void beep()
{
	HC138_C=1;
	HC138_B=0;
	HC138_A=1;
	P0=0x40;
	Delay1000ms();
	P0=0X00;	
}

void relay()
{
	HC138_C=1;
	HC138_B=0;
	HC138_A=1;
	P0=0x10;
	Delay1000ms();
	P0=0X00;
}

void main()
{
	Initsys();
	while(1)
	{
		beep();
		relay();
	}
}